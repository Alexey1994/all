org 0x7c00

mov ax, cs
mov ss, ax
mov sp, 0x8000

call read_next_sector
call read_next_sector
call read_next_sector
call read_next_sector

;разрешить A20
in al, 0x92
or al, 2
out 0x92, al

cli

;очистить прерывания
mov eax, default_interrupt_handler
mov [default_irq_low], ax
shr eax, 16
mov [default_irq_handler_high], ax
mov si, 0
clean_next_interrupt_description_data:
	mov eax, [default_irq_low]
	mov [si], eax
	add si, 4
	mov eax, [default_irq_high]
	mov [si], eax
	add si, 4
	cmp si, 2048
	jne clean_next_interrupt_description_data

lidt [IDT_pointer]
lgdt [GDT_pointer]

;перейти в 32 битный режим
mov eax, cr0
or eax, 1
mov cr0, eax

jmp 8:code_32

read_next_sector:
	mov ah, 42h
	mov dl, 0x80
	mov si, packet
	int 0x13
	jc read_error
	
	mov ax, [buffer_address_segment]
	add ax, 0x1000
	mov [buffer_address_segment], ax

	mov ax, [start_sector]
	add ax, 128
	mov [start_sector], ax

	ret

	read_error:
		mov ax, 0xB800
		mov ds, ax
		mov al, 1
		mov ah, 12
		mov bx, 0
		mov [ds:bx], ax
		hlt
		jmp $

	packet: 
		size                   db 16
		zero                   db 0
		number_of_sectors      dw 127
		buffer_address_offset  dw 0
		buffer_address_segment dw 0x1000
		start_sector           dq 1




use32
code_32:
	mov ax, 16
	mov ds, eax
	mov ss, eax

	mov eax, 0x60000
	mov esp, eax

	sti
	call 0x10000
no_actions:
	hlt
	jmp no_actions




align 16
GDT:
    ; dummy
    dq 0

    ; CODE (CS register = 8)
    dw 0xffff     ; размер сегмента
    dw 0          ; базовый адрес
    db 0          ; базовый адрес
    db 0b10011110 ; 1    сегмент правильный(должно быть 1)
                  ; 00   уровень привилегий(меньше - больше привилегий)
                  ; 1    если сегмент в памяти то 1
                  ; 1    сегмент исполняемый
                  ; 1    направление для сегмента данных либо возможность перехода с низких привилегий на высокие для сегмента кода(1 - разрешено, 0 - запрещено)
                  ; 1    разрешение на чтение для сегмента кода, разрешение на запись для сегмента данных
                  ; 0    бит доступа к сегменту, устанавливается процессором(рекомендуется 0)
    db 0b11001111 ; 1    гранулярность(если 0, то размер адреса равен размеру сегмента кода, если 1 то размеру сегмента кода * 4096)
                  ; 1    размер, если 0 и 64 битный режим(следующий бит) = 0, то селектор определяет 16 битный режим, если 1 - 32 битный. Если 64 битный режим равен 1, то должен быть равен 0(значение 1 зарезервировано, будет генерировать исключение)
                  ; 0    64 битный режим
                  ; 0    AVL(?)
                  ; 1111 размер сегмента
    db 0          ;      базовый адрес

    ; DATA (DS register = 16)
    dw 0xffff     ; размер сегмента
    dw 0          ; базовый адрес
    db 0          ; базовый адрес
    db 0b10010010 ; 1    сегмент правильный(должно быть 1)
                  ; 00   уровень привилегий(меньше - больше привилегий)
                  ; 1    если сегмент в памяти то 1
                  ; 0    сегмент исполняемый
                  ; 0    направление для сегмента данных либо возможность перехода с низких привилегий на высокие для сегмента кода
                  ; 1    разрешение на чтение для сегмента кода, разрешение на запись для сегмента данных
                  ; 0    бит доступа к сегменту, устанавливается процессором(рекомендуется 0)
    db 0b11001111 ; 1    гранулярность(если 0, то размер адреса равен размеру сегмента кода, если 1 то размеру сегмента кода * 4096)
                  ; 1    размер, если 0 и 64 битный режим(следующий бит) = 0, то селектор определяет 16 битный режим, если 1 - 32 битный. Если 64 битный режим равен 1, то должен быть равен 0(значение 1 зарезервировано, будет генерировать исключение)
                  ; 0    64 битный режим
                  ; 0    AVL(?)
                  ; 1111 размер сегмента
    db 0          ;      базовый адрес

GDT_pointer:
	dw $ - GDT - 1 ;размер
	dd GDT         ;адрес

IDT_pointer:
	dw 256 * 8 - 1 ;размер
	dd 0           ;адрес

default_irq_low:
	                          dw 0x0000     ;младшая часть адреса
	                          dw 0x0008     ;номер селектора
	default_irq_high:         db 0x0000     ;всегда 0
	                          db 10001110b  ;1    - если 1 то используется
	                                        ;00   - уровень привилегий
	                                        ;0    - 0 для прерываний
	                                        ;1110 - 32 битное прерывание
	default_irq_handler_high: dw 0x0000     ;старшая часть адреса

default_interrupt_handler:
	iret