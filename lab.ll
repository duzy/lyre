declare i32 @puts(i8* nocapture) nounwind
declare void @printf(i8*, ...)

@foo = constant [7 x i8] c"foobar\00", align 1
@.str = private unnamed_addr constant [13 x i8] c"hello world\0A\00"

!0 = !{ !"primative type" }
!1 = !{ !"bool", !0 }
!2 = !{ !"int", !0 }
!3 = !{ !"float", !0 }
!4 = !{ !"variant", !0 }
!5 = !{ !"node", !0 }

; Named metadata
;!a = !{i32 42, null, !"string"}
;!foo = !{!a}

define i32 @main() {
  %v = alloca { i8* }
  ; call void @printf([7 x i8] *@foo)

  %cast210 = getelementptr [13 x i8]* @.str, i64 0, i64 0
  call i32 @puts(i8* %cast210)
  ret i32 0
}

define i32 @"lyre·start"() {
  ret i32 0
}
