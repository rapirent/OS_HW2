### author:丁國騰(Kuo Teng, Ding)
### student_id:E94036209
### class:資工三乙
### Usage:
## start:
'''
make
'''
## open server:
'''
./server
'''
## open client:
'''
./client \[client_id\] \[client_name\]
'''
- client_name should be smaller than 64 characters long
### description:
## api.c:
    我使用shm_open函式建立share memory, 並利用mmap產生該memory之映射
    接著以memcpy及sprintf、sscanf來進行memory之存取，而close以及unlink則是以shm_unlink和munmap實作
## server.c:
    以fcntl來實做non_blocking io，以迴圈觀看當前getchar取得的鍵盤輸入是否為E，是E則跳出。
    並且不斷監看server之mailbox是否有信件，若是有則解讀該信件（以type欄位）並做出相對應的動作
## client.c:
    以mail_open來打開client和server，若是client的執行方法錯誤，將會跳出提示
    以fcntl來實做non_blocking io，以迴圈觀看當前getchar取得的鍵盤輸入是否為E，是E則跳出。
    若是其他相對應的指令字元，則做出相對應指令的動作，並將對應的郵件以mail_send傳送至server端
    在查看輸入的同時也不斷監看client之mailbox是否有信件，若是有則解讀信件的類型並發送對應的訊息至stdout stream中
### 實作的bunus:
- multi-chatroom
- LIST
- WHISPER
