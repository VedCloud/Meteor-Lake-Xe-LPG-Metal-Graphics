use metal2vulkan::{translate_native_no_retry_constructed_with_options, passes::{Stage,TransformOptions}};
use std::collections::HashMap;
const FRAG:&str=r#"target triple = "air64_v28-apple-macosx26.5.0"
define <{<4 x float>, <4 x float>}> @frag(<4 x float> %v) {
 %a = insertvalue <{<4 x float>, <4 x float>}> undef, <4 x float> %v, 0
 %b = insertvalue <{<4 x float>, <4 x float>}> %a, <4 x float> %v, 1
 ret <{<4 x float>, <4 x float>}> %b
}
!air.fragment = !{!0}
!0 = !{ptr @frag, !1, !4}
!1 = !{!2,!3}
!2 = !{!"air.render_target", i32 0, i32 0, !"air.arg_type_name", !"float4"}
!3 = !{!"air.render_target", i32 1, i32 0, !"air.arg_type_name", !"float4"}
!4 = !{!5}
!5 = !{i32 0, !"air.fragment_input", !"generated(v)", !"air.center", !"air.arg_type_name", !"float4", !"air.arg_name", !"v"}
"#;
fn check(mask:u32) {
 let bytes=translate_native_no_retry_constructed_with_options(FRAG,Stage::Fragment,TransformOptions::default().with_a8_render_targets(mask)).expect("translate actual AIR");
 let w:Vec<u32>=bytes.chunks_exact(4).map(|x|u32::from_le_bytes(x.try_into().unwrap())).collect();assert_eq!(w[0],0x07230203);
 let mut loc=HashMap::new();let mut shuffle=HashMap::new();let mut stores=Vec::new();let mut i=5;
 while i<w.len(){let n=(w[i]>>16) as usize;let op=w[i]&65535;assert!(n>0&&i+n<=w.len());let a=&w[i+1..i+n];
  if op==71&&a.len()>=3&&a[1]==30 {loc.insert(a[0],a[2]);}
  if op==79 {shuffle.insert(a[1],a[4..].to_vec());}
  if op==62 {stores.push((a[0],a[1]));}i+=n;
 }
 let mut checked=0;
 for (ptr,value) in stores {if let Some(location)=loc.get(&ptr){let alpha=shuffle.get(&value).is_some_and(|x|x==&vec![3,3,3,3]);assert_eq!(alpha,(mask>>location)&1!=0,"location {location} mask {mask}");checked+=1;}}
 assert_eq!(checked,2,"both independently decorated colour outputs were checked");
}
#[test]fn latitude_a8_outputs_only_selected_attachments(){for mask in [0,1,2,3,1u32<<31]{check(mask);}}
#[test]fn latitude_a8_zero_mask_preserves_binary(){let a=translate_native_no_retry_constructed_with_options(FRAG,Stage::Fragment,TransformOptions::default()).unwrap();let b=translate_native_no_retry_constructed_with_options(FRAG,Stage::Fragment,TransformOptions::default().with_a8_render_targets(0)).unwrap();assert_eq!(a,b);}
