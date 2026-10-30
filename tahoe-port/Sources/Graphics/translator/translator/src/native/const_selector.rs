//! Constant selector arguments: specialize an internal callee for a call that passes a pointer to a constant.
//!
//! Apple's GL compiler lowers a GLSL function that takes a `sampler2D` parameter into a callee that receives every
//! texture/sampler pair of the stage plus a selector: a pointer to an `i32` the caller stored the sampler's unit
//! index into. The callee loads it, `switch`es on it and merges the pairs with `phi`s, so the sample call's texture and
//! sampler operands are phis whose exact resources cannot be recovered (Minecraft's `sample_lightmap(Sampler2, uv)`).
//! The caller's selector is a constant (`store i32 0, ptr %a` just before the call), so each such call gets a clone
//! of the callee with the selector's loads replaced by that constant, the constant `switch`es turned into branches,
//! the blocks this leaves unreachable dropped, and the phis left with one incoming value replaced by it.
//!
//! Text-based like the other AIR normalizations; any shape it does not recognize leaves the module unchanged.

use super::lex::split_top_level;
use std::borrow::Cow;
use std::collections::{HashMap, HashSet};

const MAX_SPECIALIZATIONS: usize = 64;

pub(super) fn specialize_constant_selector_calls(san_ll: &str) -> Cow<'_, str> {
    if !san_ll.contains("switch ") {
        return Cow::Borrowed(san_ll);
    }
    let mut lines: Vec<String> = san_ll.lines().map(str::to_string).collect();
    let mut changed = false;
    for _ in 0..MAX_SPECIALIZATIONS {
        if !specialize_one(&mut lines) {
            break;
        }
        changed = true;
    }
    if !changed {
        return Cow::Borrowed(san_ll);
    }
    let mut out = lines.join("\n");
    if san_ll.ends_with('\n') {
        out.push('\n');
    }
    Cow::Owned(out)
}

/// A function definition: `lines[header]` is the `define` line, `lines[end]` its closing `}`.
struct FuncSpan {
    header: usize,
    end: usize,
    name: String,
    internal: bool,
}

fn function_spans(lines: &[String]) -> Vec<FuncSpan> {
    let mut spans = Vec::new();
    let mut i = 0;
    while i < lines.len() {
        let line = &lines[i];
        if line.starts_with("define") && line.trim_end().ends_with('{') {
            let mut j = i + 1;
            while j < lines.len() && lines[j] != "}" {
                j += 1;
            }
            if j == lines.len() {
                break;
            }
            if let Some(name) = defined_name(line) {
                spans.push(FuncSpan {
                    header: i,
                    end: j,
                    internal: line.split_whitespace().any(|t| t == "internal" || t == "private"),
                    name,
                });
            }
            i = j + 1;
        } else {
            i += 1;
        }
    }
    spans
}

/// `@name` or `@"quoted name"` of a `define` line.
fn defined_name(line: &str) -> Option<String> {
    let at = line.find('@')?;
    let rest = &line[at..];
    if rest[1..].starts_with('"') {
        let close = rest[2..].find('"')? + 2;
        Some(rest[..=close].to_string())
    } else {
        let end = rest[1..]
            .find(|c: char| !(c.is_ascii_alphanumeric() || "._$-".contains(c)))
            .map_or(rest.len(), |e| e + 1);
        Some(rest[..end].to_string())
    }
}

/// The name with `suffix` appended inside the quotes of a quoted name.
fn suffixed_name(name: &str, suffix: &str) -> String {
    match name.strip_suffix('"') {
        Some(open) => format!("{open}{suffix}\""),
        None => format!("{name}{suffix}"),
    }
}

fn is_ident_char(c: char) -> bool {
    c.is_ascii_alphanumeric() || c == '_' || c == '.' || c == '$' || c == '-'
}

/// Byte offsets of each occurrence of the local `%name` as a whole token in `line`.
fn token_offsets(line: &str, local: &str) -> Vec<usize> {
    let mut out = Vec::new();
    let mut from = 0;
    while let Some(pos) = line[from..].find(local) {
        let at = from + pos;
        let after = line[at + local.len()..].chars().next();
        let before = line[..at].chars().next_back();
        if !after.is_some_and(is_ident_char) && !before.is_some_and(|c| is_ident_char(c) || c == '%') {
            out.push(at);
        }
        from = at + local.len();
    }
    out
}

fn replace_token(line: &str, local: &str, with: &str) -> String {
    let offsets = token_offsets(line, local);
    if offsets.is_empty() {
        return line.to_string();
    }
    let mut out = String::with_capacity(line.len());
    let mut last = 0;
    for at in offsets {
        out.push_str(&line[last..at]);
        out.push_str(with);
        last = at + local.len();
    }
    out.push_str(&line[last..]);
    out
}

/// `%r = <rhs>`: the result name and the right-hand side.
fn split_def(line: &str) -> Option<(&str, &str)> {
    let t = line.trim();
    let eq = t.find(" = ")?;
    let name = t[..eq].trim();
    name.starts_with('%').then(|| (name, t[eq + 3..].trim()))
}

/// A call's callee and argument chunks (`<type> [attrs] <value>`), for `call` lines only.
fn parse_call(line: &str) -> Option<(String, Vec<String>, usize, usize)> {
    let t = line.trim_start();
    let rhs_start = match split_def(line) {
        Some(_) => line.find(" = ")? + 3,
        None => line.len() - t.len(),
    };
    let rhs = &line[rhs_start..];
    let call_at = rhs.find("call ")?;
    if !rhs[..call_at].trim().split_whitespace().all(|w| matches!(w, "tail" | "notail" | "musttail")) {
        return None;
    }
    let after = &rhs[call_at + 5..];
    let at = after.find('@')?;
    if after[..at].contains('(') {
        return None;
    }
    let callee_rest = &after[at..];
    let callee_len = if callee_rest[1..].starts_with('"') {
        callee_rest[2..].find('"')? + 3
    } else {
        callee_rest[1..].find(|c: char| !is_ident_char(c)).map_or(callee_rest.len(), |e| e + 1)
    };
    let callee = callee_rest[..callee_len].to_string();
    if !callee_rest[callee_len..].starts_with('(') {
        return None;
    }
    let open = rhs_start + call_at + 5 + at + callee_len;
    let close = matching_paren(line, open)?;
    let args = split_top_level(&line[open + 1..close], ',')
        .into_iter()
        .map(|a| a.trim().to_string())
        .filter(|a| !a.is_empty())
        .collect();
    Some((callee, args, open, close))
}

fn matching_paren(s: &str, open: usize) -> Option<usize> {
    let mut depth = 0usize;
    for (i, c) in s[open..].char_indices() {
        match c {
            '(' => depth += 1,
            ')' => {
                depth -= 1;
                if depth == 0 {
                    return Some(open + i);
                }
            }
            _ => {}
        }
    }
    None
}

/// The parameter names of a `define` line, in order (`None` for an unnamed parameter).
fn param_names(header: &str) -> Option<Vec<Option<String>>> {
    let at = header.find('@')?;
    let open = header[at..].find('(')? + at;
    let close = matching_paren(header, open)?;
    let inner = header[open + 1..close].trim();
    if inner.is_empty() {
        return Some(Vec::new());
    }
    Some(
        split_top_level(inner, ',')
            .into_iter()
            .map(|p| {
                let last = p.trim().rsplit(char::is_whitespace).next()?;
                last.starts_with('%').then(|| last.to_string())
            })
            .collect(),
    )
}

/// Finds one call that passes a constant selector and specializes it. Returns whether the module changed.
fn specialize_one(lines: &mut Vec<String>) -> bool {
    let spans = function_spans(lines);
    let by_name: HashMap<&str, &FuncSpan> = spans.iter().map(|s| (s.name.as_str(), s)).collect();
    for caller in &spans {
        let body = caller.header + 1..caller.end;
        for (alloca, constant, call_line) in constant_selector_calls(lines, body) {
            let Some((callee, args, _, _)) = parse_call(&lines[call_line]) else {
                continue;
            };
            let Some(target) = by_name.get(callee.as_str()) else {
                continue;
            };
            if !target.internal || target.name == caller.name {
                continue;
            }
            let Some(index) = args.iter().position(|a| a.rsplit(char::is_whitespace).next() == Some(alloca.as_str()))
            else {
                continue;
            };
            let Some(params) = param_names(&lines[target.header]) else {
                continue;
            };
            let Some(Some(param)) = params.get(index) else {
                continue;
            };
            let clone_name = suffixed_name(&target.name, &format!(".sel{index}.{constant}"));
            let clone_exists = by_name.contains_key(clone_name.as_str());
            let clone = if clone_exists {
                None
            } else {
                match specialized_clone(&lines[target.header..=target.end], param, constant, &target.name, &clone_name) {
                    Some(c) => Some(c),
                    None => continue,
                }
            };
            // Point the call at the clone.
            let line = &lines[call_line];
            let (_, _, open, _) = parse_call(line).expect("parsed above");
            let callee_start = open - callee.len();
            lines[call_line] = format!("{}{}{}", &line[..callee_start], clone_name, &line[open..]);
            if let Some(clone) = clone {
                let insert_at = target.end + 1;
                let mut block = vec![String::new()];
                block.extend(clone);
                lines.splice(insert_at..insert_at, block);
            }
            remove_if_uncalled(lines, &target.name);
            return true;
        }
    }
    false
}

/// `(alloca, constant, call line)` for each call in `body` that passes an `alloca i32` whose only other use is one
/// `store i32 <constant>` earlier in the same block.
fn constant_selector_calls(lines: &[String], body: std::ops::Range<usize>) -> Vec<(String, i64, usize)> {
    let mut out = Vec::new();
    for i in body.clone() {
        let Some((name, rhs)) = split_def(&lines[i]) else {
            continue;
        };
        if !rhs.starts_with("alloca i32,") && rhs != "alloca i32" {
            continue;
        }
        let mut store: Option<(usize, i64)> = None;
        let mut calls = Vec::new();
        let mut ok = true;
        for j in body.clone() {
            if j == i || token_offsets(&lines[j], name).is_empty() {
                continue;
            }
            let t = lines[j].trim();
            if let Some(rest) = t.strip_prefix("store i32 ") {
                let chunks = split_top_level(rest, ',');
                let value = chunks.first().map(|v| v.trim());
                let target = chunks.get(1).and_then(|p| p.trim().rsplit(char::is_whitespace).next());
                match (value.and_then(|v| v.parse::<i64>().ok()), target) {
                    (Some(c), Some(t)) if t == name && store.is_none() => store = Some((j, c)),
                    _ => ok = false,
                }
            } else if let Some((_, args, _, _)) = parse_call(&lines[j]) {
                // Passed as a plain `ptr %a` argument and nowhere else on the line.
                let passes = args.iter().filter(|a| a.rsplit(char::is_whitespace).next() == Some(name)).count();
                if passes == 1 && token_offsets(&lines[j], name).len() == 1 {
                    calls.push(j);
                } else {
                    ok = false;
                }
            } else {
                ok = false;
            }
        }
        let Some((store_line, constant)) = store else {
            continue;
        };
        if !ok {
            continue;
        }
        for call in calls {
            if call > store_line && same_block(lines, store_line, call) {
                out.push((name.to_string(), constant, call));
            }
        }
    }
    out
}

fn is_label_line(line: &str) -> Option<&str> {
    if line.starts_with(' ') || line.starts_with('\t') {
        return None;
    }
    let colon = line.find(':')?;
    let label = &line[..colon];
    (!label.is_empty() && !label.contains(char::is_whitespace) && !label.starts_with(';')).then_some(label)
}

fn is_terminator(line: &str) -> bool {
    let t = line.trim_start();
    t.starts_with("br ") || t.starts_with("switch ") || t.starts_with("ret") || t == "unreachable"
}

fn same_block(lines: &[String], from: usize, to: usize) -> bool {
    (from + 1..to).all(|k| is_label_line(&lines[k]).is_none() && !is_terminator(&lines[k]))
}

/// The callee's text renamed to `clone_name`, with `param`'s loads replaced by `constant` and folded; `None` if the
/// parameter has a use other than `load i32` or nothing folds.
fn specialized_clone(
    func: &[String],
    param: &str,
    constant: i64,
    name: &str,
    clone_name: &str,
) -> Option<Vec<String>> {
    let header = func[0].replacen(name, clone_name, 1);
    if header == func[0] {
        return None;
    }
    let mut body: Vec<String> = func[1..func.len() - 1].to_vec();
    let mut loaded = Vec::new();
    for line in &body {
        if token_offsets(line, param).is_empty() {
            continue;
        }
        let (result, rhs) = split_def(line)?;
        let rest = rhs.strip_prefix("load i32, ")?;
        let ptr = split_top_level(rest, ',').first()?.trim().rsplit(char::is_whitespace).next()?.to_string();
        if ptr != param {
            return None;
        }
        loaded.push(result.to_string());
    }
    if loaded.is_empty() {
        return None;
    }
    let c = constant.to_string();
    body.retain(|line| !split_def(line).is_some_and(|(r, _)| loaded.iter().any(|l| l == r)));
    for l in &loaded {
        for line in body.iter_mut() {
            *line = replace_token(line, l, &c);
        }
    }
    if !fold_constant_switches(&mut body) {
        return None;
    }
    prune_unreachable_blocks(&mut body)?;
    let mut out = vec![header];
    out.extend(body);
    out.push("}".to_string());
    Some(out)
}

/// `switch i32 <constant>, label %d [ i32 v, label %l ... ]` (on one or several lines) becomes `br label %target`.
fn fold_constant_switches(body: &mut Vec<String>) -> bool {
    let mut changed = false;
    let mut i = 0;
    while i < body.len() {
        let t = body[i].trim_start();
        let Some(rest) = t.strip_prefix("switch ") else {
            i += 1;
            continue;
        };
        let mut end = i;
        let mut text = rest.to_string();
        while !text.contains(']') && end + 1 < body.len() {
            end += 1;
            text.push(' ');
            text.push_str(body[end].trim());
        }
        let Some(target) = constant_switch_target(&text) else {
            i = end + 1;
            continue;
        };
        let indent: String = body[i].chars().take_while(|c| c.is_whitespace()).collect();
        body.splice(i..=end, [format!("{indent}br label {target}")]);
        changed = true;
        i += 1;
    }
    changed
}

fn constant_switch_target(text: &str) -> Option<String> {
    let (head, cases) = text.split_once('[')?;
    let cases = cases.split(']').next()?;
    let mut parts = split_top_level(head, ',');
    let cond = parts.first()?.trim().rsplit(char::is_whitespace).next()?.parse::<i64>().ok()?;
    let default = parts.pop()?.trim().strip_prefix("label ")?.trim().to_string();
    let words: Vec<&str> = cases.split_whitespace().collect();
    // pairs of `<ty> <value>, label <dest>`
    let mut k = 0;
    while k + 3 < words.len() {
        let value = words[k + 1].trim_end_matches(',');
        if words[k + 2] != "label" {
            return None;
        }
        if value.parse::<i64>().ok()? == cond {
            return Some(words[k + 3].trim_end_matches(',').to_string());
        }
        k += 4;
    }
    Some(default)
}

/// Drops blocks unreachable from the entry, removes phi arms whose block no longer branches to the phi's block, and
/// replaces phis left with one incoming value by that value. `None` if the block structure isn't recognized.
fn prune_unreachable_blocks(body: &mut Vec<String>) -> Option<()> {
    // blocks: (label, first line, end line exclusive); the entry block may be unlabeled
    let mut blocks: Vec<(String, usize, usize)> = Vec::new();
    let mut start = 0;
    let mut label = String::from("<entry>");
    for (i, line) in body.iter().enumerate() {
        if let Some(l) = is_label_line(line) {
            if i > start || label != "<entry>" {
                blocks.push((label.clone(), start, i));
            }
            label = format!("%{l}");
            start = i;
        }
    }
    blocks.push((label, start, body.len()));
    let index: HashMap<&str, usize> = blocks.iter().enumerate().map(|(i, b)| (b.0.as_str(), i)).collect();
    let mut successors: Vec<Vec<usize>> = Vec::with_capacity(blocks.len());
    for (_, s, e) in &blocks {
        let term: String = body[*s..*e]
            .iter()
            .skip_while(|l| !is_terminator(l))
            .map(|l| l.trim())
            .collect::<Vec<_>>()
            .join(" ");
        let words: Vec<&str> = term.split_whitespace().collect();
        let mut succ = Vec::new();
        for w in words.windows(2) {
            if w[0] == "label" {
                succ.push(*index.get(w[1].trim_end_matches([',', ']']))?);
            }
        }
        successors.push(succ);
    }
    let mut reachable = vec![false; blocks.len()];
    let mut stack = vec![0usize];
    while let Some(b) = stack.pop() {
        if !std::mem::replace(&mut reachable[b], true) {
            stack.extend(&successors[b]);
        }
    }
    // predecessors among reachable blocks, by label
    let mut preds: HashMap<usize, HashSet<&str>> = HashMap::new();
    for (b, succ) in successors.iter().enumerate() {
        if reachable[b] {
            for s in succ {
                preds.entry(*s).or_default().insert(blocks[b].0.as_str());
            }
        }
    }
    let block_of_line = |line: usize| blocks.iter().position(|(_, s, e)| (*s..*e).contains(&line));
    let mut keep = vec![true; body.len()];
    for (b, r) in blocks.iter().zip(&reachable) {
        if !r {
            keep[b.1..b.2].iter_mut().for_each(|k| *k = false);
        }
    }
    let mut substitutions: Vec<(String, String)> = Vec::new();
    let mut rewritten: Vec<(usize, String)> = Vec::new();
    for (i, line) in body.iter().enumerate() {
        if !keep[i] {
            continue;
        }
        let Some((result, rhs)) = split_def(line) else {
            continue;
        };
        let Some(rest) = rhs.strip_prefix("phi ") else {
            continue;
        };
        let Some(first_arm) = rest.find('[') else {
            continue;
        };
        let ty = rest[..first_arm].trim().to_string();
        let result = result.to_string();
        // `[ v, %b ], [ w, %c ]`: each top-level chunk is one bracketed arm
        let chunks = split_top_level(&rest[first_arm..], ',');
        let arms: Vec<(String, String)> = chunks
            .iter()
            .filter_map(|arm| {
                let inner = arm.trim().strip_prefix('[')?.strip_suffix(']')?;
                let (v, b) = inner.rsplit_once(',')?;
                Some((v.trim().to_string(), b.trim().to_string()))
            })
            .collect();
        if arms.len() != chunks.len() {
            return None;
        }
        let incoming = block_of_line(i).and_then(|b| preds.get(&b));
        let live: Vec<&(String, String)> =
            arms.iter().filter(|(_, b)| incoming.is_some_and(|p| p.contains(b.as_str()))).collect();
        if live.len() == arms.len() {
            continue;
        }
        if live.is_empty() {
            return None;
        }
        if live.len() == 1 {
            substitutions.push((result, live[0].0.clone()));
            keep[i] = false;
        } else {
            let indent: String = line.chars().take_while(|c| c.is_whitespace()).collect();
            let arms = live.iter().map(|(v, b)| format!("[ {v}, {b} ]")).collect::<Vec<_>>().join(", ");
            rewritten.push((i, format!("{indent}{result} = phi {ty} {arms}")));
        }
    }
    for (i, line) in rewritten {
        body[i] = line;
    }
    let mut out: Vec<String> = body.iter().zip(&keep).filter(|(_, k)| **k).map(|(l, _)| l.clone()).collect();
    for (from, to) in &substitutions {
        for line in out.iter_mut() {
            *line = replace_token(line, from, to);
        }
    }
    *body = out;
    Some(())
}

/// Whether `line` mentions the global `name` itself (not a longer name it is a prefix of).
fn refers_to_global(line: &str, name: &str) -> bool {
    let mut from = 0;
    while let Some(pos) = line[from..].find(name) {
        let end = from + pos + name.len();
        if name.ends_with('"') || !line[end..].chars().next().is_some_and(is_ident_char) {
            return true;
        }
        from = end;
    }
    false
}

/// Removes an internal function no call refers to any more.
fn remove_if_uncalled(lines: &mut Vec<String>, name: &str) {
    let spans = function_spans(lines);
    let Some(span) = spans.iter().find(|s| s.name == name) else {
        return;
    };
    let referenced = lines
        .iter()
        .enumerate()
        .any(|(i, l)| (i < span.header || i > span.end) && refers_to_global(l, name));
    if !referenced {
        lines.drain(span.header..=span.end);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    const SAMPLE_LIGHTMAP: &str = "\
define internal void @\"#lm\"(ptr %sel, ptr addrspace(1) %t0, ptr addrspace(1) %t1, ptr addrspace(2) %s0, ptr addrspace(2) %s1, ptr %out) #1 {
block:
  %23 = load i32, ptr %sel, align 4
  switch i32 %23, label %afterSwitch [
    i32 1, label %case
  ]

case:                                             ; preds = %block
  br label %afterSwitch

afterSwitch:                                      ; preds = %case, %block
  %24 = phi i32 [ 0, %block ], [ 1, %case ]
  %25 = phi ptr addrspace(1) [ %t0, %block ], [ %t1, %case ]
  %26 = phi ptr addrspace(2) [ %s0, %block ], [ %s1, %case ]
  %34 = call { <4 x float>, i8 } @air.sample_texture_2d.v4f32(ptr addrspace(1) %25, ptr addrspace(2) %26, <2 x float> zeroinitializer, i1 false, <2 x i32> zeroinitializer, i1 true, float 0.000000e+00, float 0.000000e+00, i32 0) #1
  %idx = add i32 %24, 0
  ret void
}

define void @main(ptr addrspace(1) %a, ptr addrspace(1) %b, ptr addrspace(2) %c, ptr addrspace(2) %d) {
entry:
  %86 = alloca i32, align 4
  %out = alloca <4 x float>, align 16
  store i32 0, ptr %86, align 4
  call void @\"#lm\"(ptr %86, ptr addrspace(1) %a, ptr addrspace(1) %b, ptr addrspace(2) %c, ptr addrspace(2) %d, ptr %out)
  ret void
}
";

    #[test]
    fn constant_sampler_selector_specializes_the_callee() {
        let out = specialize_constant_selector_calls(SAMPLE_LIGHTMAP).into_owned();
        assert!(out.contains("call void @\"#lm.sel0.0\"(ptr %86"), "{out}");
        assert!(!out.contains("define internal void @\"#lm\"("), "the uncalled original is gone: {out}");
        assert!(!out.contains("switch"), "{out}");
        assert!(!out.contains("phi "), "{out}");
        assert!(!out.contains("\ncase:"), "{out}");
        assert!(out.contains("@air.sample_texture_2d.v4f32(ptr addrspace(1) %t0, ptr addrspace(2) %s0,"), "{out}");
        assert!(out.contains("%idx = add i32 0, 0"), "{out}");
    }

    #[test]
    fn selector_one_takes_the_case_arm() {
        let src = SAMPLE_LIGHTMAP.replace("store i32 0, ptr %86", "store i32 1, ptr %86");
        let out = specialize_constant_selector_calls(&src).into_owned();
        assert!(out.contains("@\"#lm.sel0.1\""), "{out}");
        assert!(out.contains("@air.sample_texture_2d.v4f32(ptr addrspace(1) %t1, ptr addrspace(2) %s1,"), "{out}");
        assert!(out.contains("\ncase:"), "the taken block stays: {out}");
        assert!(!out.contains("phi "), "{out}");
    }

    #[test]
    fn a_selector_stored_twice_is_left_alone() {
        let src = SAMPLE_LIGHTMAP.replace(
            "  store i32 0, ptr %86, align 4\n",
            "  store i32 0, ptr %86, align 4\n  store i32 1, ptr %86, align 4\n",
        );
        assert_eq!(specialize_constant_selector_calls(&src), src);
    }

    #[test]
    fn a_selector_the_callee_writes_is_left_alone() {
        let src = SAMPLE_LIGHTMAP.replace(
            "  %23 = load i32, ptr %sel, align 4\n",
            "  %23 = load i32, ptr %sel, align 4\n  store i32 5, ptr %sel, align 4\n",
        );
        assert_eq!(specialize_constant_selector_calls(&src), src);
    }

    #[test]
    fn two_call_sites_with_different_selectors_get_two_clones() {
        let src = SAMPLE_LIGHTMAP.replace(
            "  call void @\"#lm\"(ptr %86, ptr addrspace(1) %a, ptr addrspace(1) %b, ptr addrspace(2) %c, ptr addrspace(2) %d, ptr %out)\n  ret void\n}\n",
            "  call void @\"#lm\"(ptr %86, ptr addrspace(1) %a, ptr addrspace(1) %b, ptr addrspace(2) %c, ptr addrspace(2) %d, ptr %out)\n  %87 = alloca i32, align 4\n  store i32 1, ptr %87, align 4\n  call void @\"#lm\"(ptr %87, ptr addrspace(1) %a, ptr addrspace(1) %b, ptr addrspace(2) %c, ptr addrspace(2) %d, ptr %out)\n  ret void\n}\n",
        );
        let out = specialize_constant_selector_calls(&src).into_owned();
        assert!(out.contains("call void @\"#lm.sel0.0\"(ptr %86"), "{out}");
        assert!(out.contains("call void @\"#lm.sel0.1\"(ptr %87"), "{out}");
        assert!(!out.contains("define internal void @\"#lm\"("), "{out}");
    }

    #[test]
    fn a_module_without_switches_is_borrowed() {
        let src = "define void @k() {\nentry:\n  ret void\n}\n";
        assert!(matches!(specialize_constant_selector_calls(src), Cow::Borrowed(_)));
    }
}
