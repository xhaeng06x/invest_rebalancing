#include <stdio.h>
#define MAX 10

// [오류 수정 1] 구조체 정의를 함수 원형보다 위로 올렸습니다.
typedef struct {
    long int stock_price[MAX];
    char stock_name[MAX][20];
} stock;

typedef struct {
    long int bond, gold, cash, all, all_stock;
    stock stock1;
} money;

// 이제 컴파일러가 money 구조체를 알기 때문에 오류가 나지 않습니다.
void invest(money* p_money);
void rebal(money* p_money);

int main() {
    int n;
    money money1;
    money* p_money;
    p_money = &money1;

    while (1) {
        // 안내 메시지 추가 (선택 사항)
        printf("1. 현재 보유, 2 시뮬  (그 외 종료): ");
        scanf_s("%d", &n);

        switch (n) {
        case 1:
            invest(p_money);
            break; // [권장] switch문 탈출을 위해 break 추가
        case 2: rebal(p_money); break;
        default:
            return 0; // 1이 아니면 종료하도록 처리
        }
    }
    return 0;
}

void invest(money* p_money) {
    int num = 0; int all_stock = 0; int all = 0;
    for (int i = 0; i < MAX; i++) {
        printf("%d번째 주식 이름과 평가금액 입력 (종료는 가격에 0): ", i + 1);
        scanf_s("%s %ld", p_money->stock1.stock_name[i], 20, &p_money->stock1.stock_price[i]);

        // [오류 수정 2] 인덱스 [i]를 사용하여 현재 입력된 값을 검사
        if (p_money->stock1.stock_price[i] == 0) {
            num = i;
            break;
        }
    }

    //채권 입력
    printf("\n채권 가격 입력 ");
    scanf_s("%d", &p_money->bond);
    printf("\n금 현물 ETF 가격 입력 ");
    scanf_s("%d", &p_money->gold);
    for (int i = 0; i < num; i++) {
        all_stock += p_money->stock1.stock_price[i];
    }
    all = all_stock + p_money->bond + p_money->gold;
    p_money->all = all;
    p_money->all_stock = all_stock;
    printf("주식 : %lf %%, 채권 : %lf %% , 금 : %lf %%\n", (double)all_stock/all *100, (double)p_money->bond/all*100, (double)p_money->gold / all * 100);
}
void rebal(money* p_money) {
    double a = (double)p_money->all_stock * (3.0 / 5.0), b = (double)p_money->all_stock * (2.0 / 5.0);
    printf("주식대비 채권, 금의 필요량\n채권: %lf원\n금: %lf원\n", a - p_money->bond, b - p_money->gold);
}