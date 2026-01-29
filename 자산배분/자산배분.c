#include <stdio.h>
#include <string.h>
#define MAX 11

// [오류 수정 1] 구조체 정의를 함수 원형보다 위로 올렸습니다.
typedef struct {
    long int stock_price[MAX];
    char stock_name[MAX][20];
} stock;

typedef struct {
    long int bond, gold, all, all_stock;
    stock stock1;
} money;

// 이제 컴파일러가 money 구조체를 알기 때문에 오류가 나지 않습니다.
void invest(money* p_money, int *pnum);
void rebal(money* p_money);
void rebal2(money* p_money);
void f_save(money* p_money, int *pnum);
void f_load(money* p_money, int* pnum);


int main() {
    
    int n; int num = 0;
    money money1;
    money* p_money;
    memset(&money1, 0, sizeof(money));
    p_money = &money1;
    int* pnum;
    pnum = &num;
    while (1) {
        // 안내 메시지 추가 (선택 사항)
        
        printf("1. 자산입력(초기화)\n2. 시뮬(주식대비 채권 금차이)\n3. 총 자산 기준 시뮬\n4. 저장\n5. 불러오기\n(그 외 종료)\n>>  ");
        scanf("%d", &n);

        switch (n) {
        case 1:
            invest(p_money, pnum);
            break; // [권장] switch문 탈출을 위해 break 추가
        case 2: rebal(p_money); break;
        case 3: rebal2(p_money); break;
        case 4: f_save(p_money, pnum); break;
        case 5: f_load(p_money, pnum); break;
        default:
            return 0; // 1이 아니면 종료하도록 처리
        }
    }
    return 0;
}

void invest(money* p_money, int *pnum) {
    int all_stock = 0; int all = 0;
    for (int i = 0; i < MAX; i++) {
        printf("%d번째 주식 이름과 평가금액 입력 (종료는 가격에 0): ", i + 1);
        scanf("%s %ld", p_money->stock1.stock_name[i], &p_money->stock1.stock_price[i]);

        // [오류 수정 2] 인덱스 [i]를 사용하여 현재 입력된 값을 검사
        if (p_money->stock1.stock_price[i] == 0) {
            *pnum = i;
            break;
        }

    }
    //채권 입력
    printf("\n채권 가격 입력 ");
    scanf("%d", &p_money->bond);
    printf("\n금 현물 ETF 가격 입력 ");
    scanf("%d", &p_money->gold);
    for (int i = 0; i < *pnum; i++) {
        all_stock += p_money->stock1.stock_price[i];
    }
    all = all_stock + p_money->bond + p_money->gold;
    p_money->all = all;
    p_money->all_stock = all_stock;
    printf("주식 : %lf %%, 채권 : %lf %% , 금 : %lf %%\n", (double)all_stock/all *100, (double)p_money->bond/all*100, (double)p_money->gold / all * 100);
    
}
void rebal(money* p_money) {
    double a = (double)p_money->all_stock * (3.0 / 6.0), b = (double)p_money->all_stock * (1.0 / 6.0);
    printf("주식대비 채권, 금의 필요량\n채권: %lf원\n금: %lf원\n", a - p_money->bond, b - p_money->gold);
}
void rebal2(money* p_money) {
    double a = (double)p_money->all *0.3, b = (double)p_money->all * 0.1, c=(double)p_money->all *0.6;
    printf("주식(60%): %lf원\n채권: %lf원\n금: %lf원\n", c, a , b );
    printf("현재 비율과 비교\n주식(60%): %lf원\n채권(30%): %lf원\n금(10%): %lf원\n", c-p_money->all_stock, a - p_money->bond, b - p_money->gold);
}
void f_save(money* p_money, int *pnum) {
    FILE* fp;
    fp = fopen("save.txt", "w");
    if (fp == NULL) {
        printf("파일열기 실패\n");
        return;
    }
    fprintf(fp, "총 자산\t전체 주식\t채권\t\t금\n");
    fprintf(fp, "%8d\t%8d\t%8d\t%8d\n", p_money->all, p_money->all_stock, p_money->bond, p_money->gold);
    fprintf(fp, "%4d%%\t%8.2lf%%\t%8.2lf%%\t%8.2lf%%\n\n", 100, (double)p_money->all_stock / p_money->all * 100, (double)p_money->bond / p_money->all * 100, (double)p_money->gold / p_money->all * 100);

    for (int i = 0; i < *pnum; i++) {
        fprintf(fp, "%s\t%-8d\n", p_money->stock1.stock_name[i], p_money->stock1.stock_price[i]);
    }
    fclose(fp);
    printf("\n저장 성공!\n");
}
void f_load(money* p_money, int* pnum) {
    FILE* fp;
    char buffer[100];
    double buffer1[100][100]; // 첫 줄(헤더)을 건너뛰기 위한 임시 버퍼

    fp = fopen("save.txt", "r"); // 읽기 모드 "r"
    if (fp == NULL) {
        printf("저장된 파일(save.txt)이 없습니다.\n");
        return;
    }

    // 1. 첫 줄(한글 제목들)은 계산에 필요 없으므로 읽어서 버립니다.
    fgets(buffer, sizeof(buffer), fp);

    // 2. 두 번째 줄(총 자산 등 숫자 데이터)을 읽습니다.
    // long int 타입이므로 %ld를 사용합니다.
    fscanf(fp, "%ld %ld %ld %ld", &p_money->all, &p_money->all_stock, &p_money->bond, &p_money->gold);
    fscanf(fp, "%lf%% %lf%% %lf%% %lf%%", &buffer1[0], &buffer1[1], &buffer1[2], &buffer1[3]);
    // 3. 세 번째 줄부터 있는 개별 주식 데이터를 파일 끝(EOF)까지 읽습니다.
    *pnum = 0; // 주식 개수 초기화
    while (fscanf(fp, "%s %ld", p_money->stock1.stock_name[*pnum], &p_money->stock1.stock_price[*pnum]) != EOF) {
        (*pnum)++; // 읽을 때마다 개수 증가
        if (*pnum >= MAX) break; // 배열 최대 크기 넘지 않도록 방지
    }

    fclose(fp);
    printf("\n불러오기 성공! (총 %d개의 주식 종목 로드됨)\n", *pnum);
    printf("주식 : %ld원, 채권 : %ld원, 금 : %ld원\n", p_money->all_stock, p_money->bond, p_money->gold);
    printf("주식 : %lf %%, 채권 : %lf %% , 금 : %lf %%\n", (double)p_money->all_stock / p_money->all * 100, (double)p_money->bond / p_money->all * 100, (double)p_money->gold / p_money->all * 100);
}