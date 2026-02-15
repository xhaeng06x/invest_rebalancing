// #include <Python.h>

#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
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
  long int gold, all, all_stock, all_bond, cash;
  stock stock1;
  bond bond1;
} money;

// 이제 컴파일러가 money 구조체를 알기 때문에 오류가 나지 않습니다.
void invest(money *p_money, int *pnum);
void rebal(money *p_money);
void rebal2(money *p_money);
void rebal_stock(money *p_money, int *pnum);
void rebal_bond(money *p_money, int *pnum);
void f_save(money *p_money, int *pnum);
void f_load(money *p_money, int *pnum);
void show_chart(money *p_money, int *pnum);
void pyapi(money *p_money, int *pnum);

int main() {

  int n;
  int num[2] = {0, 0};
  money money1;
  money *p_money;
  memset(&money1, 0, sizeof(money));
  p_money = &money1;
  int *pnum = num;
  while (1) {
    // 안내 메시지 추가 (선택 사항)

    printf("\n1. 자산입력(초기화)\n2. 시뮬(주식대비 채권 금차이)\n3. 총 자산 "
           "기준 시뮬\n4. 개별주식비중보기\n5. 개별채권비중보기\n6. 저장\n7. "
           "불러오기\n8. 원형 그래프 보기\n9. (beta)한국투자 api로 "
           "불러오기\n(그 외 "
           "종료)\n>>  ");

    scanf("%d", &n);
    switch (n) {
    case 1:
      invest(p_money, pnum);
      break; // [권장] switch문 탈출을 위해 break 추가
    case 2:
      rebal(p_money);
      break;
    case 3:
      rebal2(p_money);
      break;
    case 4:
      rebal_stock(p_money, pnum);
      break;
    case 5:
      rebal_bond(p_money, pnum);
      break;
    case 6:
      f_save(p_money, pnum);
      break;
    case 7:
      f_load(p_money, pnum);
      break;
    case 8:
      show_chart(p_money, pnum);
      break;
    case 9:
      pyapi(p_money, pnum);
      break;
    default:
      return 0; // 1이 아니면 종료하도록 처리
    }
  }
  return 0;
}

void invest(money *p_money, int *pnum) {
  int all_stock = 0;
  int all = 0;
  int all_bond = 0;
  for (int i = 0; i < MAX; i++) {
    printf("%d번째 주식 이름과 평가금액, 목표비중 입력 (종료는 비중에 0입력): ",
           i + 1);
    scanf("%s %ld %d", p_money->stock1.stock_name[i],
          &p_money->stock1.stock_price[i], &p_money->stock1.stock_goal[i]);

    // [오류 수정 2] 인덱스 [i]를 사용하여 현재 입력된 값을 검사
    if (p_money->stock1.stock_goal[i] == 0) {
      *pnum = i;
      break;
    }
  }
  // 채권 입력
  for (int i = 0; i < MAX; i++) {
    printf("%d번째 채권 이름과 평가금액, 목표비중 입력 (종료는 비중에 0): ",
           i + 1);
    scanf("%s %ld %d", p_money->bond1.bond_name[i],
          &p_money->bond1.bond_price[i], &p_money->bond1.bond_goal[i]);

    // [오류 수정 2] 인덱스 [i]를 사용하여 현재 입력된 값을 검사
    if (p_money->bond1.bond_goal[i] == 0) {
      *(pnum + 1) = i;
      break;
    }
  }
  // printf("\n채권 가격 입력 ");
  // scanf("%d", &p_money->bond);
  printf("\n금 현물 ETF 가격 입력 ");
  scanf("%ld", &p_money->gold);

  printf("\n예수금 입력 :  ");
  scanf("%ld", &p_money->cash);

  for (int i = 0; i < *pnum; i++) {
    all_stock += p_money->stock1.stock_price[i];
  }
  for (int i = 0; i < *(pnum + 1); i++) {
    all_bond += p_money->bond1.bond_price[i];
  }
  all = all_stock + all_bond + p_money->gold + p_money->cash;
  p_money->all = all;
  p_money->all_stock = all_stock;
  p_money->all_bond = all_bond;
  printf("주식 : %lf %%, 채권 : %lf %% , 금 : %lf %%, 현금 : %lf %%\n",
         (double)all_stock / all * 100, (double)p_money->all_bond / all * 100,
         (double)p_money->gold / all * 100, (double)p_money->cash / all * 100);
}
void rebal(money *p_money) {
  double a = (double)p_money->all_stock * (3.0 / 6.0),
         b = (double)p_money->all_stock * (1.0 / 6.0);
  printf("주식(고정)대비 채권, 금의 필요량\n채권: %lf원\n금: %lf원\n",
         a - p_money->all_bond, b - p_money->gold);
}
void rebal2(money *p_money) {
  int select, plus, all;
  printf("\n시드를 새로 추가하시겠습니까? (1:네, 0:아니오) ");
  scanf("%d", &select);
  if (select == 1) {
    printf("\n추가할 시드를 입력하시오.");
    scanf("%d", &plus);
    all = p_money->all + plus;
  } else {
    all = p_money->all;
  }
  double a = (double)all * 0.3, b = (double)all * 0.1, c = (double)all * 0.6;
  printf("주식(60%%): %lf원\n채권: %lf원\n금: %lf원\n", c, a, b);
  printf("전체 자산 대비 과부족액\n주식(60%%): %lf원\n채권(30%%): "
         "%lf원\n금(10%%): %lf원\n",
         c - p_money->all_stock, a - p_money->all_bond, b - p_money->gold);
}
void rebal_stock(money *p_money, int *pnum) {
  printf("\n\n현재 개별 주식 종목 비중");
  for (int i = 0; i < *pnum; i++) {
    printf("\n%s(목표 %d %%) : %ld원, %.2lf %%\t전체 대비 과부족액 : %lf원",
           p_money->stock1.stock_name[i], p_money->stock1.stock_goal[i],
           p_money->stock1.stock_price[i],
           (double)p_money->stock1.stock_price[i] / p_money->all * 100,
           (double)p_money->all * p_money->stock1.stock_goal[i] / 100 -
               p_money->stock1.stock_price[i]);
  }
  printf("\n");
}
void rebal_bond(money *p_money, int *pnum) {
  printf("\n\n현재 개별 채권 종목 비중");
  for (int i = 0; i < *(pnum + 1); i++) {
    printf("\n%s(목표 %d %%) : %ld원, %.2lf %%\t    전체 대비 과부족액 : %lf원",
           p_money->bond1.bond_name[i], p_money->bond1.bond_goal[i],
           p_money->bond1.bond_price[i],
           (double)p_money->bond1.bond_price[i] / p_money->all * 100,
           (double)p_money->all * p_money->bond1.bond_goal[i] / 100 -
               p_money->bond1.bond_price[i]);
  }
  printf("\n");
}
// [수정된 f_save] 주식 개수와 채권 개수를 파일에 기록합니다.
void f_save(money *p_money, int *pnum) {
  FILE *fp = fopen("save.txt", "w");
  if (fp == NULL) {
    printf("파일열기 실패\n");
    return;
  }
  // 1. 헤더
  fprintf(fp, "총자산\t전체주식\t채권\t금\t예수금\n");
  // 2. 자산 데이터
  fprintf(fp, "%ld\t%ld\t%ld\t%ld\t%ld\n", p_money->all, p_money->all_stock,
          p_money->all_bond, p_money->gold, p_money->cash);

  // 3. [중요] 주식 개수와 채권 개수를 저장합니다.
  fprintf(fp, "%d %d\n", pnum[0], pnum[1]);

  // 4. 주식 리스트 저장
  for (int i = 0; i < pnum[0]; i++) {
    fprintf(fp, "%s %ld %d\n", p_money->stock1.stock_name[i],
            p_money->stock1.stock_price[i], p_money->stock1.stock_goal[i]);
  }

  // 5. 채권 리스트 저장
  for (int i = 0; i < pnum[1]; i++) {
    fprintf(fp, "%s %ld %d\n", p_money->bond1.bond_name[i],
            p_money->bond1.bond_price[i], p_money->bond1.bond_goal[i]);
  }

  fclose(fp);
  printf("\n저장 성공! (주식: %d개, 채권: %d개)\n", pnum[0], pnum[1]);
}

// [수정된 f_load] 저장된 개수를 읽어와서 반복문을 돌립니다.
void f_load(money *p_money, int *pnum) {
  FILE *fp;
  char buffer[200]; // 줄 건너뛰기용 버퍼

  fp = fopen("save.txt", "r");
  if (fp == NULL) {
    printf("저장된 파일(save.txt)이 없습니다.\n");
    return;
  }

  // 1. 첫 줄(헤더) 건너뛰기
  fgets(buffer, sizeof(buffer), fp);

  // 2. 전체 자산 데이터 읽기
  fscanf(fp, "%ld %ld %ld %ld %ld", &p_money->all, &p_money->all_stock,
         &p_money->all_bond, &p_money->gold, &p_money->cash);

  // 3. [중요] 주식 개수와 채권 개수 읽기
  int s_cnt = 0, b_cnt = 0;
  fscanf(fp, "%d %d", &s_cnt, &b_cnt);
  pnum[0] = s_cnt; // 주식 개수 복구
  pnum[1] = b_cnt; // 채권 개수 복구

  // 4. 주식 개수만큼 읽기
  for (int i = 0; i < s_cnt; i++) {
    fscanf(fp, "%s %ld %d", p_money->stock1.stock_name[i],
           &p_money->stock1.stock_price[i], &p_money->stock1.stock_goal[i]);
  }

  // 5. 채권 개수만큼 읽기
  for (int i = 0; i < b_cnt; i++) {
    fscanf(fp, "%s %ld %d", p_money->bond1.bond_name[i],
           &p_money->bond1.bond_price[i], &p_money->bond1.bond_goal[i]);
  }

  fclose(fp);
  int all_stock = 0;
  int all = 0;
  int all_bond = 0;
  for (int i = 0; i < MAX; i++) {
    all_stock += p_money->stock1.stock_price[i];
  }
  for (int i = 0; i < MAX; i++) {
    all_bond += p_money->bond1.bond_price[i];
  }
  all = all_stock + all_bond + p_money->gold + p_money->cash;

  if (p_money->all != all) {
    printf("\n자산에 변동이 생겨 업데이트 합니다.\n");
    p_money->all = all;
    p_money->all_stock = all_stock;
    p_money->all_bond = all_bond;
  }
  printf("\n불러오기 성공!\n");
  printf("주식 종목: %d개 로드됨\n", pnum[0]);
  printf("채권 종목: %d개 로드됨\n", pnum[1]);

  if (p_money->all > 0) {
    printf(
        "현재 상태 -> 주식 : %ld원, 채권 : %ld원, 금 : %ld원, 예수금 : %ld원\n",
        p_money->all_stock, p_money->all_bond, p_money->gold, p_money->cash);
    printf("\n현재 비중 -> 주식 : %.2lf %%, 채권 : %.2lf %%, 금 : %.2lf %%, "
           "예수금 : %.2lf %%\n",
           (double)p_money->all_stock / p_money->all * 100,
           (double)p_money->all_bond / p_money->all * 100,
           (double)p_money->gold / p_money->all * 100,
           (double)p_money->cash / p_money->all * 100);
  }
}
void show_chart(money *p_money, int *pnum) {
  if (p_money->all <= 0) {
    printf("데이터가 없어 그래프를 표시할 수 없습니다.\n");
    return;
  }
  FILE *fp = fopen("draw_chart.py", "w");
  if (fp == NULL)
    return;

  fprintf(fp, "import matplotlib.pyplot as plt\n");
  fprintf(fp, "import numpy as np\n");

  // 데이터 준비
  fprintf(fp, "group_names = ['Stock', 'Bond', 'Gold']\n");
  fprintf(fp, "group_size = [%ld, %ld, %ld]\n", p_money->all_stock,
          p_money->all_bond, p_money->gold);

  fprintf(fp, "subgroup_names = [");
  // 주식 이름
  for (int i = 0; i < pnum[0]; i++)
    fprintf(fp, "'%s', ", p_money->stock1.stock_name[i]);
  // 채권 이름
  for (int i = 0; i < pnum[1]; i++)
    fprintf(fp, "'%s', ", p_money->bond1.bond_name[i]);
  // 금 이름
  fprintf(fp, "'Gold']\n");

  fprintf(fp, "subgroup_size = [");
  // 주식 가격
  for (int i = 0; i < pnum[0]; i++)
    fprintf(fp, "%ld, ", p_money->stock1.stock_price[i]);
  // 채권 가격
  for (int i = 0; i < pnum[1]; i++)
    fprintf(fp, "%ld, ", p_money->bond1.bond_price[i]);
  // 금 가격
  fprintf(fp, "%ld]\n", p_money->gold);

  // 색상 설정 (Nested Pie Chart)
  fprintf(fp, "a, b, c = [plt.cm.Reds, plt.cm.Blues, plt.cm.YlOrBr]\n");

  // Outer Ring Colors
  fprintf(fp, "outer_colors = []\n");
  if (pnum[0] > 0)
    fprintf(fp, "outer_colors += [a(x) for x in np.linspace(0.3, 0.9, %d)]\n",
            pnum[0]);
  if (pnum[1] > 0)
    fprintf(fp, "outer_colors += [b(x) for x in np.linspace(0.3, 0.9, %d)]\n",
            pnum[1]);
  fprintf(fp, "outer_colors += [c(0.6)]\n"); // Gold color

  // Inner Ring Colors
  fprintf(fp, "inner_colors = [a(0.6), b(0.6), c(0.6)]\n");

  fprintf(fp, "fig, ax = plt.subplots(figsize=(14, 10))\n");
  fprintf(fp, "ax.axis('equal')\n");

  // Inner Ring
  fprintf(fp, "mypie, _ = ax.pie(group_size, radius=1.0, labels=group_names, "
              "labeldistance=0.6, colors=inner_colors, textprops={'fontsize': "
              "12}, wedgeprops=dict(width=0.4, edgecolor='white'))\n");

  // Outer Ring: 라벨을 직접 배치하기 위해 labels=None으로 설정
  fprintf(
      fp,
      "mypie2, _ = ax.pie(subgroup_size, radius=1.4, labels=None, "
      "colors=outer_colors, wedgeprops=dict(width=0.4, edgecolor='white'))\n");

  // [개선] 라벨 겹침 방지를 위한 지그재그 배치 (Staggered Labels)
  fprintf(fp, "for i, w in enumerate(mypie2):\n");
  fprintf(fp, "    ang = (w.theta2 - w.theta1)/2. + w.theta1\n");
  fprintf(
      fp,
      "    rad = 1.35 if i %% 2 == 0 else 1.55\n"); // 반지름을 번갈아가며 설정
  fprintf(fp, "    x = rad * np.cos(np.deg2rad(ang))\n");
  fprintf(fp, "    y = rad * np.sin(np.deg2rad(ang))\n");
  // [개선] 폰트 크기 확대
  fprintf(fp, "    font_size = 12 if subgroup_size[i]/sum(subgroup_size) > "
              "0.02 else 10\n");
  fprintf(fp, "    ax.text(x, y, subgroup_names[i], ha='center', va='center', "
              "fontsize=font_size, fontweight='bold')\n");

  fprintf(fp, "plt.title('Asset Allocation Breakdown', pad=20, fontsize=16)\n");

  // Add text box with details
  fprintf(fp, "textstr = ''\n");
  fprintf(fp, "total = sum(group_size)\n");
  fprintf(fp, "textstr += f'Total: {total:,} won\\n\\n'\n");
  fprintf(fp, "textstr += f'Stock: {group_size[0]/total*100:.1f}%%\\n'\n");
  // Add individual stocks
  for (int i = 0; i < pnum[0]; i++) {
    fprintf(fp,
            "textstr += f'  - %s: {subgroup_size[%d]/total*100:.1f}%%\\n'\n",
            p_money->stock1.stock_name[i], i);
  }

  fprintf(fp, "textstr += f'Bond: {group_size[1]/total*100:.1f}%%\\n'\n");
  // Add individual bonds
  for (int i = 0; i < pnum[1]; i++) {
    fprintf(fp,
            "textstr += f'  - %s: {subgroup_size[%d]/total*100:.1f}%%\\n'\n",
            p_money->bond1.bond_name[i], pnum[0] + i);
  }

  fprintf(fp, "textstr += f'Gold: {group_size[2]/total*100:.1f}%%\\n'\n");

  fprintf(fp, "plt.gcf().text(0.02, 0.5, textstr, fontsize=12, "
              "bbox=dict(facecolor='white', alpha=0.5))\n");
  fprintf(fp, "plt.subplots_adjust(left=0.3)\n"); // Make room for text

  fprintf(fp, "plt.savefig('chart.png')\n");
  fclose(fp);

  printf("그래프를 생성 중입니다...\n");
  system("python3 draw_chart.py");
  printf("그래프를 생성했습니다.\n");
}

void pyapi(money *p_money, int *pnum) {
  printf("파이썬 API를 실행합니다...\n");
  // .venv의 python을 사용하도록 변경
  system("../.venv/bin/python apiopen.py");
  FILE *fp = fopen("data.json", "r");
  if (fp == NULL) {
    printf("파일을 열 수 없습니다.\n");
    return;
  }

  // 2. 파일 크기 확인 및 전체 내용을 읽을 버퍼 준비
  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *json_buffer = (char *)malloc(file_size + 1);
  fread(json_buffer, 1, file_size, fp);
  json_buffer[file_size] = '\0'; // 문자열 끝에 널 문자 추가
  fclose(fp);
  cJSON *json = cJSON_Parse(json_buffer);

  // 파싱에 실패했을 경우 안전하게 종료
  if (json == NULL) {
    printf("JSON 파싱 에러 발생!\n");
    free(json_buffer); // 에러가 나도 메모리는 돌려줘야 합니다.
    return;
  }
  double amount = 0.0;
  char stocks[20];
  cJSON *price = cJSON_GetObjectItemCaseSensitive(json, "amount");
  if (cJSON_IsNumber(price)) {
    amount = price->valuedouble;
  }
  cJSON *stocks1 = cJSON_GetObjectItemCaseSensitive(json, "ticker");
  if (cJSON_IsString(stocks1) && (stocks1->valuestring != NULL)) {
    strcpy(stocks, stocks1->valuestring);
  }
  cJSON_Delete(json); // 파싱된 JSON 객체 삭제
  free(json_buffer);  // 파일을 읽어왔던 임시 버퍼 삭제
  printf("%lf\n", amount);
  printf("%s", stocks);
  // 파일은 다 읽었으니 닫아줍니다.
}
