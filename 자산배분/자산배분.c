#include <stdio.h>
#include <string.h>
#define MAX 20

// [오류 수정 1] 구조체 정의를 함수 원형보다 위로 올렸습니다.
typedef struct {
    long int stock_price[MAX];
    char stock_name[MAX][40];
    int stock_goal[MAX];
} stock;
typedef struct {
    long int bond_price[MAX];
    char bond_name[MAX][40];
    int bond_goal[MAX];
} bond;
typedef struct {
    long int gold, all, all_stock, all_bond;
    stock stock1; bond bond1;
} money;

// 이제 컴파일러가 money 구조체를 알기 때문에 오류가 나지 않습니다.
void invest(money* p_money, int *pnum);
void rebal(money* p_money);
void rebal2(money* p_money);
void rebal_stock(money* p_money, int *pnum);
void rebal_bond(money*p_money, int *pnum);
void f_save(money* p_money, int *pnum);
void f_load(money* p_money, int* pnum);


int main() {
    
    int n; int num[2]={0, 0};
    money money1;
    money* p_money;
    memset(&money1, 0, sizeof(money));
    p_money = &money1;
    int *pnum = num;
    while (1) {
        // 안내 메시지 추가 (선택 사항)
        
        printf("1. 자산입력(초기화)\n2. 시뮬(주식대비 채권 금차이)\n3. 총 자산 기준 시뮬\n4. 개별주식비중보기\n5. 개별채권비중보기\n6. 저장\n7. 불러오기\n(그 외 종료)\n>>  ");
        scanf("%d", &n);
        switch (n) {
        case 1:
            invest(p_money, pnum);
            break; // [권장] switch문 탈출을 위해 break 추가
        case 2: rebal(p_money); break;
        case 3: rebal2(p_money); break;
        case 4: rebal_stock(p_money, pnum);break;
        case 5: rebal_bond(p_money, pnum);break;
        case 6: f_save(p_money, pnum); break;
        case 7: f_load(p_money, pnum); break;
        default:
            return 0; // 1이 아니면 종료하도록 처리
        }
    }
    return 0;
}

void invest(money* p_money, int *pnum) {
    int all_stock = 0; int all = 0; int all_bond=0;
    for (int i = 0; i < MAX; i++) {
        printf("%d번째 주식 이름과 평가금액, 목표비중 입력 (종료는 가격에 0): ", i + 1);
        scanf("%s %ld %d", p_money->stock1.stock_name[i], &p_money->stock1.stock_price[i], &p_money->stock1.stock_goal[i]);

        // [오류 수정 2] 인덱스 [i]를 사용하여 현재 입력된 값을 검사
        if (p_money->stock1.stock_price[i] == 0) {
            *pnum = i;
            break;
        }

    }
    //채권 입력
    for (int i = 0; i < MAX; i++) {
        printf("%d번째 채권 이름과 평가금액, 목표비중 입력 (종료는 가격에 0): ", i + 1);
        scanf("%s %ld %d", p_money->bond1.bond_name[i], &p_money->bond1.bond_price[i], &p_money->bond1.bond_goal[i]);

        // [오류 수정 2] 인덱스 [i]를 사용하여 현재 입력된 값을 검사
        if (p_money->bond1.bond_price[i] == 0) {
            *(pnum+1) = i;
            break;
        }

    }
    //printf("\n채권 가격 입력 ");
    //scanf("%d", &p_money->bond);
    printf("\n금 현물 ETF 가격 입력 ");
    scanf("%ld", &p_money->gold);
    for (int i = 0; i < *pnum; i++) {
        all_stock += p_money->stock1.stock_price[i];
    }
    for (int i = 0; i < *(pnum+1); i++) {
        all_bond += p_money->bond1.bond_price[i];
    }
    all = all_stock + all_bond + p_money->gold;
    p_money->all = all;
    p_money->all_stock = all_stock;
    p_money->all_bond = all_bond;
    printf("주식 : %lf %%, 채권 : %lf %% , 금 : %lf %%\n", (double)all_stock/all *100, (double)p_money->all_bond/all*100, (double)p_money->gold / all * 100);
    
}
void rebal(money* p_money) {
    double a = (double)p_money->all_stock * (3.0 / 6.0), b = (double)p_money->all_stock * (1.0 / 6.0);
    printf("주식(고정)대비 채권, 금의 필요량\n채권: %lf원\n금: %lf원\n", a - p_money->all_bond, b - p_money->gold);
}
void rebal2(money* p_money) {
    double a = (double)p_money->all *0.3, b = (double)p_money->all * 0.1, c=(double)p_money->all *0.6;
    printf("주식(60%%): %lf원\n채권: %lf원\n금: %lf원\n", c, a , b );
    printf("전체 자산 대비 과부족액\n주식(60%%): %lf원\n채권(30%%): %lf원\n금(10%%): %lf원\n", c-p_money->all_stock, a - p_money->all_bond, b - p_money->gold);
}
void rebal_stock(money*p_money, int *pnum){
    printf("\n\n현재 개별 주식 종목 비중");
    for (int i=0; i<*pnum; i++){
        printf("\n%s(목표 %d %%) : %ld원, %.2lf %%\t전체 대비 과부족액 : %lf원", p_money->stock1.stock_name[i], p_money->stock1.stock_goal[i], p_money->stock1.stock_price[i], (double)p_money->stock1.stock_price[i]/p_money->all*100, (double)p_money->all*p_money->stock1.stock_goal[i]/100-p_money->stock1.stock_price[i]);
    }
    printf("\n");
}
void rebal_bond(money*p_money, int *pnum){
    printf("\n\n현재 개별 채권 종목 비중");
    for (int i=0; i<*(pnum+1); i++){
        printf("\n%s(목표 %d %%) : %ld원, %.2lf %%\t    전체 대비 과부족액 : %lf원", p_money->bond1.bond_name[i], p_money->bond1.bond_goal[i], p_money->bond1.bond_price[i], (double)p_money->bond1.bond_price[i]/p_money->all*100, (double)p_money->all*p_money->bond1.bond_goal[i]/100-p_money->bond1.bond_price[i]);
    }
    printf("\n");
}
// [수정된 f_save] 주식 개수와 채권 개수를 파일에 기록합니다.
void f_save(money* p_money, int *pnum) {
    FILE* fp = fopen("save.txt", "w");
    if (fp == NULL) {
        printf("파일열기 실패\n");
        return;
    }
    // 1. 헤더
    fprintf(fp, "총자산\t전체주식\t채권\t\t금\n");
    // 2. 자산 데이터
    fprintf(fp, "%ld\t%ld\t%ld\t%ld\n", p_money->all, p_money->all_stock, p_money->all_bond, p_money->gold);
    
    // 3. [중요] 주식 개수와 채권 개수를 저장합니다.
    fprintf(fp, "%d %d\n", pnum[0], pnum[1]); 

    // 4. 주식 리스트 저장
    for (int i = 0; i < pnum[0]; i++) {
        fprintf(fp, "%s %ld %d\n", p_money->stock1.stock_name[i], p_money->stock1.stock_price[i], p_money->stock1.stock_goal[i]);
    }
    
    // 5. 채권 리스트 저장
    for (int i = 0; i < pnum[1]; i++) {
        fprintf(fp, "%s %ld %d\n", p_money->bond1.bond_name[i], p_money->bond1.bond_price[i], p_money->bond1.bond_goal[i]);
    }

    fclose(fp);
    printf("\n저장 성공! (주식: %d개, 채권: %d개)\n", pnum[0], pnum[1]);
}

// [수정된 f_load] 저장된 개수를 읽어와서 반복문을 돌립니다.
void f_load(money* p_money, int* pnum) {
    FILE* fp;
    char buffer[200]; // 줄 건너뛰기용 버퍼

    fp = fopen("save.txt", "r");
    if (fp == NULL) {
        printf("저장된 파일(save.txt)이 없습니다.\n");
        return;
    }

    // 1. 첫 줄(헤더) 건너뛰기
    fgets(buffer, sizeof(buffer), fp);

    // 2. 전체 자산 데이터 읽기
    fscanf(fp, "%ld %ld %ld %ld", &p_money->all, &p_money->all_stock, &p_money->all_bond, &p_money->gold);

    // 3. [중요] 주식 개수와 채권 개수 읽기
    int s_cnt = 0, b_cnt = 0;
    fscanf(fp, "%d %d", &s_cnt, &b_cnt);
    pnum[0] = s_cnt; // 주식 개수 복구
    pnum[1] = b_cnt; // 채권 개수 복구

    // 4. 주식 개수만큼 읽기
    for (int i = 0; i < s_cnt; i++) {
        fscanf(fp, "%s %ld %d", p_money->stock1.stock_name[i], &p_money->stock1.stock_price[i], &p_money->stock1.stock_goal[i]);
    }

    // 5. 채권 개수만큼 읽기
    for (int i = 0; i < b_cnt; i++) {
        fscanf(fp, "%s %ld %d", p_money->bond1.bond_name[i], &p_money->bond1.bond_price[i], &p_money->bond1.bond_goal[i]);
    }

    fclose(fp);
    int all_stock = 0; int all = 0; int all_bond=0;
    for (int i = 0; i < MAX; i++) {
        all_stock += p_money->stock1.stock_price[i];
    }
    for (int i = 0; i < MAX; i++) {
        all_bond += p_money->bond1.bond_price[i];
    }
    all = all_stock + all_bond + p_money->gold;
    
    if (p_money->all != all){
        printf("\n자산에 변동이 생겨 업데이트 합니다.\n");
        p_money->all = all;
        p_money->all_stock = all_stock;
        p_money->all_bond = all_bond;
    }
    printf("\n불러오기 성공!\n");
    printf("주식 종목: %d개 로드됨\n", pnum[0]);
    printf("채권 종목: %d개 로드됨\n", pnum[1]);
    
    if (p_money->all > 0) {
        printf("현재 상태 -> 주식 : %ld원, 채권 : %ld원, 금 : %ld원\n", p_money->all_stock, p_money->all_bond, p_money->gold);
        printf("\n현재 비중 -> 주식 : %.2lf %%, 채권 : %.2lf %%, 금 : %.2lf %%\n", (double)p_money->all_stock/p_money->all*100, (double)p_money->all_bond/p_money->all*100, (double)p_money->gold/p_money->all*100);
    }
}