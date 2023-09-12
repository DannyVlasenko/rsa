ExitProcess PROTO
MessageBoxA PROTO
GetModuleHandleA PROTO
RegisterClassA PROTO
CreateWindowExA PROTO
ShowWindow PROTO
GetMessageA PROTO
TranslateMessage PROTO
DispatchMessageA PROTO
DefWindowProcA PROTO
PostQuitMessage PROTO

.data
myText					db "Hello World", 0
myCaption				db "Message Box Testing", 0
MainWindowClassName		db "MASM Window Class", 0
MainWindowName			db "MASM Test", 0

.code
showInfoMsg PROC
	push rbp       ;   save caller's rbp
	mov rbp, rsp   ;   use rbp as a frame pointer	
	sub rsp, 20h   ;   reserved as Win32 parameters' shadow area
	
	mov r8, rdx
	mov rdx, rcx
	mov rcx, 0
	mov r9, 40h
	call MessageBoxA
	
	mov rsp, rbp   ;   restore stack pointer
	pop rbp        ;   restore caller's rbp
	ret
showInfoMsg ENDP

windowProc PROC
	push rbp
	mov rbp, rsp
	sub rsp, 20h

	call DefWindowProcA
	jmp L_EXIT

L_EXIT:
    mov rsp, rbp   
    pop rbp        
    ret
windowProc ENDP

windowLoop PROC
	push rbp       
    mov rbp, rsp   
    sub rsp, 20h

	mov r12, rcx

	; MSG struct
	sub rsp, 48
	
L_MSG_LOOP:
	mov rcx, rsp
	mov rdx, r12
	mov r8, 0
	mov r9, 0
	call GetMessageA
	test rax, rax
	je L_EXIT

	mov rcx, rsp
	call TranslateMessage
	mov rcx, rsp
	call DispatchMessageA
	jmp L_MSG_LOOP

L_EXIT:
	mov rsp, rbp									
    pop rbp											
    ret
windowLoop ENDP

createMainWindow PROC	
	mov qword ptr [rsp+24], r8
	mov qword ptr [rsp+16], rdx
	mov qword ptr [rsp+8], rcx
	push rbp 	
    mov rbp, rsp  
    sub rsp, 32 
	
	; Getting hInstance
	mov rcx, 0
	call GetModuleHandleA
	mov r12, rax

	; Registering window class
	sub rsp, 80
	mov rcx, 0
	mov QWORD PTR [rsp],	rcx
	lea rdx, windowProc
	mov QWORD PTR [rsp+8],	rdx						; lpfnWndProc
	mov QWORD PTR [rsp+16], rcx
	mov QWORD PTR [rsp+24], r12						; hInstance
	mov QWORD PTR [rsp+32], rcx		
	mov QWORD PTR [rsp+40], rcx
	mov QWORD PTR [rsp+48], rcx
	mov QWORD PTR [rsp+56], rcx
	lea rdx, MainWindowClassName
	mov QWORD PTR [rsp+64], rdx						; lpszClassName
	mov QWORD PTR [rsp+72], rcx
	mov rcx, rsp
	call RegisterClassA

	; Creating a window
	mov rcx, 0
	lea rdx, MainWindowClassName
	lea r8,  MainWindowName
	mov r9,  0CF0000h								; WS_OVERLAPPED_WINDOW
	add rsp, 16
	mov rax, 80000000h								; CW_USEDEFAULT
	mov QWORD PTR [rsp],    rax
	mov QWORD PTR [rsp+8],  rax
	mov QWORD PTR [rsp+16],	rax
	mov QWORD PTR [rsp+24], rax
	mov rax, 0	
	mov QWORD PTR [rsp+32], rax
	mov QWORD PTR [rsp+40], rax
	mov QWORD PTR [rsp+48], r12						; hInstance
	mov QWORD PTR [rsp+56], rax
	call CreateWindowExA
	
	mov rsp, rbp
	pop rbp	
	ret
createMainWindow ENDP

main PROC
	sub rsp, 28h									; shadow area
	
	call createMainWindow
	mov r13, rax
	mov rcx, r13
	mov rdx, 1
	call ShowWindow

	mov rcx, r13
	call windowLoop

	mov rcx, 0
	call ExitProcess
main ENDP

END