; ModuleID = '/mnt/castor/seas_home/n/nmavuso/ese5320_final_project/fpga_accelerate/solution1/.autopilot/db/a.g.ld.5.gdce.bc'
source_filename = "llvm-link"
target datalayout = "e-m:e-i64:64-i128:128-i256:256-i512:512-i1024:1024-i2048:2048-i4096:4096-n8:16:32:64-S128-v16:16-v24:32-v32:32-v48:64-v96:128-v192:256-v256:256-v512:512-v1024:1024"
target triple = "fpga64-xilinx-none"

; Function Attrs: noinline
define void @apatb_lzw_fpga_ir(i8* %input, i32 %input_size, i32* %output_code, i32* %output_size, i8* %output, i32* %output_length) local_unnamed_addr #0 {
entry:
  %input_copy = alloca [256 x i8], align 512
  %output_code_copy = alloca [256 x i32], align 512
  %output_size_copy = alloca i32, align 512
  %output_copy = alloca [256 x i8], align 512
  %output_length_copy = alloca i32, align 512
  %0 = bitcast i8* %input to [256 x i8]*
  %1 = bitcast i32* %output_code to [256 x i32]*
  %2 = bitcast i8* %output to [256 x i8]*
  call fastcc void @copy_in([256 x i8]* %0, [256 x i8]* nonnull align 512 %input_copy, [256 x i32]* %1, [256 x i32]* nonnull align 512 %output_code_copy, i32* %output_size, i32* nonnull align 512 %output_size_copy, [256 x i8]* %2, [256 x i8]* nonnull align 512 %output_copy, i32* %output_length, i32* nonnull align 512 %output_length_copy)
  %3 = getelementptr inbounds [256 x i8], [256 x i8]* %input_copy, i32 0, i32 0
  %4 = getelementptr inbounds [256 x i32], [256 x i32]* %output_code_copy, i32 0, i32 0
  %5 = getelementptr inbounds [256 x i8], [256 x i8]* %output_copy, i32 0, i32 0
  call void @apatb_lzw_fpga_hw(i8* %3, i32 %input_size, i32* %4, i32* %output_size_copy, i8* %5, i32* %output_length_copy)
  call fastcc void @copy_out([256 x i8]* %0, [256 x i8]* nonnull align 512 %input_copy, [256 x i32]* %1, [256 x i32]* nonnull align 512 %output_code_copy, i32* %output_size, i32* nonnull align 512 %output_size_copy, [256 x i8]* %2, [256 x i8]* nonnull align 512 %output_copy, i32* %output_length, i32* nonnull align 512 %output_length_copy)
  ret void
}

; Function Attrs: argmemonly noinline
define internal fastcc void @copy_in([256 x i8]* readonly, [256 x i8]* noalias align 512, [256 x i32]* readonly, [256 x i32]* noalias align 512, i32* readonly, i32* noalias align 512, [256 x i8]* readonly, [256 x i8]* noalias align 512, i32* readonly, i32* noalias align 512) unnamed_addr #1 {
entry:
  call fastcc void @onebyonecpy_hls.p0a256i8([256 x i8]* align 512 %1, [256 x i8]* %0)
  call fastcc void @onebyonecpy_hls.p0a256i32([256 x i32]* align 512 %3, [256 x i32]* %2)
  call fastcc void @onebyonecpy_hls.p0i32(i32* align 512 %5, i32* %4)
  call fastcc void @onebyonecpy_hls.p0a256i8([256 x i8]* align 512 %7, [256 x i8]* %6)
  call fastcc void @onebyonecpy_hls.p0i32(i32* align 512 %9, i32* %8)
  ret void
}

; Function Attrs: argmemonly noinline
define internal fastcc void @onebyonecpy_hls.p0a256i8([256 x i8]* noalias align 512, [256 x i8]* noalias readonly) unnamed_addr #2 {
entry:
  %2 = icmp eq [256 x i8]* %0, null
  %3 = icmp eq [256 x i8]* %1, null
  %4 = or i1 %2, %3
  br i1 %4, label %ret, label %copy

copy:                                             ; preds = %entry
  br label %for.loop

for.loop:                                         ; preds = %for.loop, %copy
  %for.loop.idx1 = phi i64 [ 0, %copy ], [ %for.loop.idx.next, %for.loop ]
  %dst.addr = getelementptr [256 x i8], [256 x i8]* %0, i64 0, i64 %for.loop.idx1
  %src.addr = getelementptr [256 x i8], [256 x i8]* %1, i64 0, i64 %for.loop.idx1
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %dst.addr, i8* align 1 %src.addr, i64 1, i1 false)
  %for.loop.idx.next = add nuw nsw i64 %for.loop.idx1, 1
  %exitcond = icmp ne i64 %for.loop.idx.next, 256
  br i1 %exitcond, label %for.loop, label %ret

ret:                                              ; preds = %for.loop, %entry
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i1) #3

; Function Attrs: argmemonly noinline
define internal fastcc void @onebyonecpy_hls.p0a256i32([256 x i32]* noalias align 512, [256 x i32]* noalias readonly) unnamed_addr #2 {
entry:
  %2 = icmp eq [256 x i32]* %0, null
  %3 = icmp eq [256 x i32]* %1, null
  %4 = or i1 %2, %3
  br i1 %4, label %ret, label %copy

copy:                                             ; preds = %entry
  br label %for.loop

for.loop:                                         ; preds = %for.loop, %copy
  %for.loop.idx3 = phi i64 [ 0, %copy ], [ %for.loop.idx.next, %for.loop ]
  %dst.addr.gep1 = getelementptr [256 x i32], [256 x i32]* %0, i64 0, i64 %for.loop.idx3
  %5 = bitcast i32* %dst.addr.gep1 to i8*
  %src.addr.gep2 = getelementptr [256 x i32], [256 x i32]* %1, i64 0, i64 %for.loop.idx3
  %6 = bitcast i32* %src.addr.gep2 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %5, i8* align 1 %6, i64 4, i1 false)
  %for.loop.idx.next = add nuw nsw i64 %for.loop.idx3, 1
  %exitcond = icmp ne i64 %for.loop.idx.next, 256
  br i1 %exitcond, label %for.loop, label %ret

ret:                                              ; preds = %for.loop, %entry
  ret void
}

; Function Attrs: argmemonly noinline
define internal fastcc void @onebyonecpy_hls.p0i32(i32* noalias align 512, i32* noalias readonly) unnamed_addr #2 {
entry:
  %2 = icmp eq i32* %0, null
  %3 = icmp eq i32* %1, null
  %4 = or i1 %2, %3
  br i1 %4, label %ret, label %copy

copy:                                             ; preds = %entry
  %5 = bitcast i32* %0 to i8*
  %6 = bitcast i32* %1 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %5, i8* align 1 %6, i64 4, i1 false)
  br label %ret

ret:                                              ; preds = %copy, %entry
  ret void
}

; Function Attrs: argmemonly noinline
define internal fastcc void @copy_out([256 x i8]*, [256 x i8]* noalias readonly align 512, [256 x i32]*, [256 x i32]* noalias readonly align 512, i32*, i32* noalias readonly align 512, [256 x i8]*, [256 x i8]* noalias readonly align 512, i32*, i32* noalias readonly align 512) unnamed_addr #4 {
entry:
  call fastcc void @onebyonecpy_hls.p0a256i8([256 x i8]* %0, [256 x i8]* align 512 %1)
  call fastcc void @onebyonecpy_hls.p0a256i32([256 x i32]* %2, [256 x i32]* align 512 %3)
  call fastcc void @onebyonecpy_hls.p0i32(i32* %4, i32* align 512 %5)
  call fastcc void @onebyonecpy_hls.p0a256i8([256 x i8]* %6, [256 x i8]* align 512 %7)
  call fastcc void @onebyonecpy_hls.p0i32(i32* %8, i32* align 512 %9)
  ret void
}

declare void @apatb_lzw_fpga_hw(i8*, i32, i32*, i32*, i8*, i32*)

define void @lzw_fpga_hw_stub_wrapper(i8*, i32, i32*, i32*, i8*, i32*) #5 {
entry:
  %6 = bitcast i8* %0 to [256 x i8]*
  %7 = bitcast i32* %2 to [256 x i32]*
  %8 = bitcast i8* %4 to [256 x i8]*
  call void @copy_out([256 x i8]* null, [256 x i8]* %6, [256 x i32]* null, [256 x i32]* %7, i32* null, i32* %3, [256 x i8]* null, [256 x i8]* %8, i32* null, i32* %5)
  %9 = bitcast [256 x i8]* %6 to i8*
  %10 = bitcast [256 x i32]* %7 to i32*
  %11 = bitcast [256 x i8]* %8 to i8*
  call void @lzw_fpga_hw_stub(i8* %9, i32 %1, i32* %10, i32* %3, i8* %11, i32* %5)
  call void @copy_in([256 x i8]* null, [256 x i8]* %6, [256 x i32]* null, [256 x i32]* %7, i32* null, i32* %3, [256 x i8]* null, [256 x i8]* %8, i32* null, i32* %5)
  ret void
}

declare void @lzw_fpga_hw_stub(i8*, i32, i32*, i32*, i8*, i32*)

attributes #0 = { noinline "fpga.wrapper.func"="wrapper" }
attributes #1 = { argmemonly noinline "fpga.wrapper.func"="copyin" }
attributes #2 = { argmemonly noinline "fpga.wrapper.func"="onebyonecpy_hls" }
attributes #3 = { argmemonly nounwind }
attributes #4 = { argmemonly noinline "fpga.wrapper.func"="copyout" }
attributes #5 = { "fpga.wrapper.func"="stub" }

!llvm.dbg.cu = !{}
!llvm.ident = !{!0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0, !0}
!llvm.module.flags = !{!1, !2, !3}
!blackbox_cfg = !{!4}

!0 = !{!"clang version 7.0.0 "}
!1 = !{i32 2, !"Dwarf Version", i32 4}
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{}
