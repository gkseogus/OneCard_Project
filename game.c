#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>

struct card {
	int value; // 카드 숫자 (1~13)
	char suit; // 카드 문양
};
struct gameInfo{ // manager와 player간 주고 받는 데이터 형식 
    struct card cards[52]; // player가 현재 소유한 카드 정보
    int num_cards; // player가 현재 소유한 카드 개수
    struct card open_card; // 현재 open card 정보
    int manager_pid; // 데이터를 보내는 프로세스의 pid
    int player_pid; // 데이터를 받는 프로세스의 pid
};

// 카드 정보
struct card cards[52];
struct card open_card;
int top = 0;

// 메세지 큐 key IDs
int msqid_p1_down; // manager -> player1
int msqid_p1_up; // player 1 -> manager
int msqid_p2_down;// manager -> player2
int msqid_p2_up; // player 1 -> manager2

int msqid_p3_down;// manager -> player3
int msqid_p3_up; // player 1 -> manager3


// 플레이어들의 pid;
int p1_pid;
int p2_pid;

int p3_pid;


void make_cards(){
    // make cards
    printf("Make cards");
	for (int i=0;i<13;i++) {
		cards[i].value=i%13+1;
		cards[i].suit='c';
	}
	for (int i=0;i<13;i++) {
		cards[i+13].value=i%13+1;
		cards[i+13].suit='d';
	}
	for (int i=0;i<13;i++) {
		cards[i+26].value=i%13+1;
		cards[i+26].suit='h';
	}
	for (int i=0;i<13;i++) {
		cards[i+39].value=i%13+1;
		cards[i+39].suit='s';
	}
    for (int i=0;i<52;i++) {
		printf("(%c,%d) ", cards[i].suit,cards[i].value);
	}
    printf("\n\n");
}
void shuffle(struct card *cards, int num){
    srand(time(NULL));
    struct card temp;
    int randInt;
    printf("Shuffling the cards\n");
    for (int i=0; i<num-1; i++){
        randInt = rand() % (num-i) + i;
        temp = cards[i];
        cards[i] = cards[randInt];
        cards[randInt] = temp;
    }
    for (int i=0;i<52;i++) {
		printf("(%c,%d) ", cards[i].suit,cards[i].value);
	}
    printf("\n");
}

int main(void) {
    // 카드 생성
    make_cards();
    // 카드 셔플
    shuffle(cards, 52);
    printf("\n@@ cards setting---------------------------------\n");
    // message queue ID 확인
    key_t key_p1_down = 50001;
    key_t key_p1_up = 50002;
    key_t key_p2_down = 60001;
    key_t key_p2_up = 60002;
    key_t key_p3_down = 70001;
    key_t key_p3_up = 70002;

    // messge queue create
    if((msqid_p1_down=msgget(key_p1_down,IPC_CREAT|0666))==-1){return -1;}
    if((msqid_p1_up=msgget(key_p1_up,IPC_CREAT|0666))==-1){return -1;}
    if((msqid_p2_down=msgget(key_p2_down,IPC_CREAT|0666))==-1){return -1;}
    if((msqid_p2_up=msgget(key_p2_up,IPC_CREAT|0666))==-1){return -1;}

    if((msqid_p3_down=msgget(key_p3_down,IPC_CREAT|0666))==-1){return -1;}
    if((msqid_p3_up=msgget(key_p3_up,IPC_CREAT|0666))==-1){return -1;}
    // messge queue reset
    msgctl(key_p1_down, IPC_RMID, NULL);
    msgctl(key_p1_down, IPC_RMID, NULL);
    msgctl(key_p1_down, IPC_RMID, NULL);
    msgctl(key_p1_down, IPC_RMID, NULL);

    msgctl(key_p1_down, IPC_RMID, NULL);
    msgctl(key_p1_down, IPC_RMID, NULL);
    // messge queue create
    if((msqid_p1_down=msgget(key_p1_down,IPC_CREAT|0666))==-1){return -1;}
    if((msqid_p1_up=msgget(key_p1_up,IPC_CREAT|0666))==-1){return -1;}
    if((msqid_p2_down=msgget(key_p2_down,IPC_CREAT|0666))==-1){return -1;}
    if((msqid_p2_up=msgget(key_p2_up,IPC_CREAT|0666))==-1){return -1;}

    if((msqid_p3_down=msgget(key_p3_down,IPC_CREAT|0666))==-1){return -1;}
    if((msqid_p3_up=msgget(key_p3_up,IPC_CREAT|0666))==-1){return -1;}

    struct gameInfo send_p1; // 플레이어 1에게 전달할 정보
    struct gameInfo send_p2; // 플레이어 2에게 전달할 정보
    struct gameInfo receive_p1; // 플레이어 1에게 전달받을 정보
    struct gameInfo receive_p2; // 플레이어 2에게 전달받을 정보

    struct gameInfo send_p3; // 플레이어 3에게 전달할 정보
    struct gameInfo receive_p3; // 플레이어 3에게 전달받을 정보

    // top에 위치한 카드 인덱스 표시
    top = 0;
    // 각 플레이어에게 전달할 초기 게임 정보 생성
    //player 1
    send_p1.num_cards = 0;
    send_p1.manager_pid = getpid();
    send_p1.player_pid = -1;
    for (int i=top; i<6; i++){ 
        send_p1.cards[i]= cards[i];
        send_p1.num_cards ++;
        top ++;
    } 
    // player 2
    send_p2.num_cards = 0;
    send_p2.manager_pid = getpid();
    send_p2.player_pid = -1;
    int i = 0;
    for (i=0; i < 6; i++){
        send_p2.cards[i]= cards[i+top];
        send_p2.num_cards ++;   
    }
    top += i+1;

    // player 3
    send_p3.num_cards = 0;
    send_p3.manager_pid = getpid();
    send_p3.player_pid = -1;
    for (int i=0; i < 6; i++){
        send_p3.cards[i]= cards[i+top+1];
        send_p3.num_cards ++;   
    }
    top += i+2;

    // open 카드 확인 및 전달할 정보에 표시
    open_card = cards[top]; 
    send_p1.open_card = open_card;
    send_p2.open_card = open_card;

    send_p3.open_card = open_card;

    top ++;
    // 게임 정보 전달
    printf("sending first game info\n");
    if(msgsnd(msqid_p1_down, &send_p1, sizeof(struct gameInfo), 0)==-1){return 0;};
    if(msgsnd(msqid_p2_down, &send_p2, sizeof(struct gameInfo), 0)==-1){return 0;};

    if(msgsnd(msqid_p3_down, &send_p3, sizeof(struct gameInfo), 0)==-1){return 0;};

    // 플레이어의 정보 수신 --> 플레이어의 pid 저장
    printf("receive the players pids\n");
    if(msgrcv(msqid_p1_up, &receive_p1, sizeof(struct gameInfo), 0, 0)==-1){return 1;};
    if(msgrcv(msqid_p2_up, &receive_p2, sizeof(struct gameInfo), 0, 0)==-1){return 1;};

    if(msgrcv(msqid_p3_up, &receive_p3, sizeof(struct gameInfo), 0, 0)==-1){return 1;};

    p1_pid = receive_p1.player_pid;
    p2_pid = receive_p2.player_pid;

    p3_pid = receive_p3.player_pid;

    // 게임 시작.
    while(1){
        ////////////////////////////////////////////////////////////////////////
        // Player 1의 차례//////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////
        kill(p1_pid, SIGUSR1);
        printf("Player 1's turn\n");
        // 1. 업데이트된 게임 정보 전달 (즉, 현재 오픈카드 정보)
        send_p1.open_card = open_card;
        if(msgsnd(msqid_p1_down, &send_p1, sizeof(struct gameInfo), 0)==-1){return 0;};
        // 2. 플레이어 1의 게임 정보 수신
        if(msgrcv(msqid_p1_up, &receive_p1, sizeof(struct gameInfo), 0, 0)==-1){return 1;};
        // 3. 게임 정보 업데이트 & 필요시 플레이어 1에게 새로운 카드 전달
        if (receive_p1.open_card.value == open_card.value && receive_p1.open_card.suit == open_card.suit){
            //open 카드 정보가 같으면, 플레이어가 카드를 내려놓지 못한 것. --> 새로운 카드 전달.
            for(int i=0; i<receive_p1.num_cards; i++){
                send_p1.cards[i] = receive_p1.cards[i];
            }
            send_p1.cards[receive_p1.num_cards] = cards[top];
            send_p1.num_cards = receive_p1.num_cards+1;
            top ++;
            if(msgsnd(msqid_p1_down, &send_p1, sizeof(struct gameInfo), 0)==-1){return 0;};
        }
        else{
            //open 카드 정보가 다르면, 플레이어가 카드를 내려놓은 것. --> 현재 open 카드 정보 업데이트
            open_card = receive_p1.open_card;

        }
        // 4. 게임 종료 판단
        if (receive_p1.num_cards == 0){
            kill(p1_pid, SIGINT); // 승리
            kill(p2_pid, SIGQUIT); // 패배
            kill(p3_pid, SIGQUIT); // 패배
            printf("Player 1 Win\n");
            return(9);
        }
        if (receive_p1.num_cards > 13){
            kill(p1_pid, SIGQUIT); // 패배
            kill(p2_pid, SIGILL); // draw
            kill(p3_pid, SIGILL); // draw
            printf("Player 1 loser\n");
            printf("Player 2,3 draw\n");
            return(9);
        }
        if (top >= 51){
            //카드 다씀, 무승부, coin = 0 => win
            int coin = rand()&2;
            if (coin == 0){
               printf("p1 coin = 0");
               printf("\n");
               printf("p1 = win");
               printf("\n");
               printf("p2, p3 = lose");
               printf("\n");
               kill(p1_pid, SIGINT); // 승리
               kill(p2_pid, SIGQUIT); // 패배
               kill(p3_pid, SIGQUIT); // 패배
            }
            else {
               printf("p1 coin = 1");
               printf("\n");
               printf("p1 = lose");
               printf("\n");
               printf("p2,3 = draw");
               printf("\n");
               kill(p1_pid, SIGQUIT); // 패배
               kill(p2_pid, SIGILL); // draw
               kill(p3_pid, SIGILL); // draw
            }
        }

        
        ////////////////////////////////////////////////////////////////////////
        // Player 2의 차례//////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        kill(p2_pid, SIGUSR1);
        printf("Player 2's turn\n");
        // 1. 업데이트된 게임 정보 전달 (즉, 현재 오픈카드 정보)
        send_p2.open_card = open_card;
        if(msgsnd(msqid_p2_down, &send_p2, sizeof(struct gameInfo), 0)==-1){return 0;};
        // 2. 플레이어 1의 게임 정보 수신
        if(msgrcv(msqid_p2_up, &receive_p2, sizeof(struct gameInfo), 0, 0)==-1){return 1;};
        // 3. 게임 정보 업데이트 & 필요시 플레이어 1에게 새로운 카드 전달
        if (receive_p2.open_card.value == open_card.value && receive_p2.open_card.suit == open_card.suit){
            //open 카드 정보가 같으면, 플레이어가 카드를 내려놓지 못한 것. --> 새로운 카드 전달.
            for(int i=0; i<receive_p2.num_cards; i++){
                send_p2.cards[i] = receive_p2.cards[i];
            }
            send_p2.cards[receive_p2.num_cards] = cards[top];
            send_p2.num_cards = receive_p2.num_cards+1;
            top ++;
            if(msgsnd(msqid_p2_down, &send_p2, sizeof(struct gameInfo), 0)==-1){return 0;};
        }
        else{
            //open 카드 정보가 다르면, 플레이어가 카드를 내려놓은 것. --> 현재 open 카드 정보 업데이트
            open_card = receive_p2.open_card;

        }
        // 4. 게임 종료 판단
        if (receive_p2.num_cards == 0){
            kill(p2_pid, SIGINT); // 승리
            kill(p1_pid, SIGQUIT); // 패배
            kill(p3_pid, SIGQUIT); // 패배
            printf("Player 2 Win\n");
            return(9);
        }
        if (receive_p2.num_cards > 13){
            kill(p2_pid, SIGQUIT); // 패배
            kill(p1_pid, SIGILL); // draw
            kill(p3_pid, SIGILL); // draw
            printf("Player 2 loser\n");
            printf("Player 1,3 draw\n");
            return(9);
        }

        if (top >= 51){
            //카드 다씀, 무승부, coin = 0 => win
            int coin = rand()&2;
            if (coin == 0){
               printf("p2 coin = 0");
               printf("\n");
               printf("p2 = win");
               printf("\n");
               printf("p1, p3 = lose");
               printf("\n");
               kill(p2_pid, SIGINT); // 승리
               kill(p1_pid, SIGQUIT); // 패배
               kill(p3_pid, SIGQUIT); // 패배
            }
            else {
               printf("p2 coin = 1");
               printf("\n");
               printf("p2 = lose");
               printf("\n");
               printf("p1,3 = draw");
               printf("\n");
               kill(p2_pid, SIGQUIT); // 패배
               kill(p1_pid, SIGILL); // draw
               kill(p3_pid, SIGILL); // draw
            }
        }

        ////////////////////////////////////////////////////////////////////////
        // Player 3의 차례//////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        kill(p3_pid, SIGUSR1);
        printf("Player 3's turn\n");
        // 1. 업데이트된 게임 정보 전달 (즉, 현재 오픈카드 정보)
        send_p3.open_card = open_card;
        if(msgsnd(msqid_p3_down, &send_p3, sizeof(struct gameInfo), 0)==-1){return 0;};
        // 2. 플레이어 3의 게임 정보 수신
        if(msgrcv(msqid_p3_up, &receive_p3, sizeof(struct gameInfo), 0, 0)==-1){return 1;};
        // 3. 게임 정보 업데이트 & 필요시 플레이어 1에게 새로운 카드 전달
        if (receive_p3.open_card.value == open_card.value && receive_p3.open_card.suit == open_card.suit){
            //open 카드 정보가 같으면, 플레이어가 카드를 내려놓지 못한 것. --> 새로운 카드 전달.
            for(int i=0; i<receive_p3.num_cards; i++){
                send_p3.cards[i] = receive_p3.cards[i];
            }
            send_p3.cards[receive_p3.num_cards] = cards[top];
            send_p3.num_cards = receive_p3.num_cards+1;
            top ++;
            if(msgsnd(msqid_p3_down, &send_p3, sizeof(struct gameInfo), 0)==-1){return 0;};
        }
        else{
            //open 카드 정보가 다르면, 플레이어가 카드를 내려놓은 것. --> 현재 open 카드 정보 업데이트
            open_card = receive_p3.open_card;

        }
        // 4. 게임 종료 판단
        if (receive_p3.num_cards == 0){
            kill(p3_pid, SIGINT); // 승리
            kill(p1_pid, SIGQUIT); // 패배
            kill(p2_pid, SIGQUIT); // 패배
            printf("Player 3 Win\n");
            return(9);
        }
        if (receive_p3.num_cards > 13){
            kill(p3_pid, SIGQUIT); // 패배
            kill(p1_pid, SIGILL); // draw
            kill(p2_pid, SIGILL); // draw
            printf("Player 3 loser\n");
            printf("Player 1,2 draw\n");
            return(9);
        }
        if (top >= 51){
            //카드 다씀, 무승부, coin = 0 => win
            int coin = rand()&2;
            if (coin == 0){
               printf("p3 coin = 0");
               printf("\n");
               printf("p3 = win");
               printf("\n");
               printf("p1, p2 = lose");
               printf("\n");
               kill(p3_pid, SIGINT); // 승리
               kill(p2_pid, SIGQUIT); // 패배
               kill(p1_pid, SIGQUIT); // 패배
            }
            else {
               printf("p3 coin = 1");
               printf("\n");
               printf("p3 = lose");
               printf("\n");
               printf("p1, p2 = draw");
               printf("\n");
               kill(p3_pid, SIGQUIT); // 패배
               kill(p2_pid, SIGILL); // draw
               kill(p1_pid, SIGILL); // draw
            }
        }
    }
    return 9;
}
