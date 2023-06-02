# 원카드 게임

![forthebadge](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)

3인이서 진행하는 원카드 게임 입니다. 

<br/><br/>

# 목차

- 소개
  - 프로젝트 개요
  - 목적과 목표
  - 기술 스택 및 환경
- 설치가이드
  - 설치방법
  - 설정 및 구성
- 사용법
  - 프로젝트 실행 방법
  - 주요 기능 
- 라이선스
  - 프로젝트에 대한 라이선스 정보

<br/><br/>

# 소개

<br/>

## 프로젝트 개요

대학교 과제로 주어진 프로젝트였으며 주제는 원카드 였습니다.

원카드의 기본 룰에 따라 저만의 방식으로 인원수 추가 혹은 룰 변경을 하였습니다.

<br/>

## 목적과 목표

나만의 새로운 원카드 시스템을 구축

<br/>

## 기술 스택 및 환경

기술 스택: c++

환경: Linux && VScode && Ubuntu

<br/><br/>

# 설치 가이드

<br/>

## 설치방법

[vscode](https://code.visualstudio.com/) 설치

[리눅스](https://www.linux.org/) 설치

[Ubuntu](https://ubuntu.com/download/desktop) 설치

<br/>

## 설정 및 구성

가상os에서 우분투를 통해 리눅스 설치 후 vsCode 실행 

<br/><br/>

# 사용법

<br/>

## 프로젝트 실행 방법

파이썬 환경변수 설정 후 주피터 노트북으로 해당 코드 실행

<br/>

## 주요 기능

<br/>

기존 포커 카드 생성 코드

```
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
```

<br/>

카드 셔플 코드

```
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
```

<br/>

카드 전달 코드

```
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
 ```
 
<br/><br/>

# 라이선스

<br/>

##  프로젝트에 대한 라이선스 정보

MIT 라이선스 사용


