from pykis.client.auth import KisAuth
from pykis.kis import PyKis
from pykis.api.account.balance import KisBalance
import json
from decimal import Decimal

print("[Python] API 서버에 접속 중...")
auth = KisAuth(
    # HTS 로그인 ID  예) soju06
    id="$3357902",
    # 앱 키  예) Pa0knAM6JLAjIa93Miajz7ykJIXXXXXXXXXX
    appkey="PSVV03ZVISC2z0Y9UZIGMTuwiKWobqMsY00i",
    # 앱 시크릿 키  예) V9J3YGPE5q2ZRG5EgqnLHn7XqbJjzwXcNpvY . . .
    secretkey="Sb+ukHtpTjZIeFIghP1S3C4n6zfOMWuMd+2TEPKoSqYBIvTW45fzw2shuUlcxl2UxtPbAlrd0cunIFlVcB7EScQ5SJSLUyhamc+r5YRRNkGSpsXJ9h8rKXu+TlcQQfDWbBiY3aajb+quwV4ic+LbwS7zYzNxxdIUFTR9iqBHy0BX29xTvxA=",
    # 앱 키와 연결된 계좌번호  예) 00000000-01
    account="44001898-01",
      #모의투자 여부
    virtual=False,
)
# 안전한 경로에 시크릿 키를 파일로 저장합니다.
auth.save("secret.json")
auth2 = KisAuth(
    # HTS 로그인 ID  예) soju06
    id="$3357902",
    # 앱 키  예) Pa0knAM6JLAjIa93Miajz7ykJIXXXXXXXXXX
    appkey="PSSFk48wAXKipEcBZzmBWrGsouNRZpxec43G",
    # 앱 시크릿 키  예) V9J3YGPE5q2ZRG5EgqnLHn7XqbJjzwXcNpvY . . .
    secretkey="y5cfBeggEcUcqDE6wTjDrRYS+4NOMRZL0iQHuX5qUS/giGIYrWBG/DQoDnfF4aNkgQpc+WIpaNhG3/B9JwZBU/J8wNDm4hP7I0PHvlJM1dF63WdS7ZYpi4rIMHfO/BPRzwjbCkN7DIa8q51ibcwMMGCqOIbw1NFqLazpP3H9S/J++h/joPo=",
    # 앱 키와 연결된 계좌번호  예) 00000000-01
    account="47238152-01",
      #모의투자 여부
    virtual=False,
)
auth2.save("secret1.json")
# 객체 생성 (한 번만 선언)
kis = PyKis("secret.json", keep_token=True)
kis1 = PyKis("secret1.json", keep_token=True)
# 잔고 확인
account = kis.account()
account1 = kis1.account()
balance: KisBalance = account.balance()
balance1: KisBalance = account1.balance()
print(balance.account_number)
print(repr(balance.stocks))
print(balance.amount)
print(balance.current_amount)
#print(repr(balance1))

class DecimalEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, Decimal):
            # C언어에서 double형으로 받기 편하게 float으로 변환
            # 정밀도가 극도로 중요하다면 str(obj)로 변환해도 됩니다.
            return float(obj) 
        return super(DecimalEncoder, self).default(obj)
#a = json.dumps(data, cls=DecimalEncoder)
#print(a)

print('[Python]데이터를 c로 보냅니다.')
def fetch_and_save():
    # 1. API에서 데이터를 받아왔다고 가정 (SGOV 같은 채권 ETF 데이터)
    data = {
      "amount" : balance.amount,
      "ticker" : "hahaha"
    }
    # 2. 데이터를 data.json 파일로 저장
    # 'w' 모드로 열어서 씁니다.
    with open("data.json", "w", encoding="utf-8") as f:
        json.dump(data, f, cls=DecimalEncoder, indent=4)
        
    print("[Python] data.json 파일 저장 완료!")

if __name__ == "__main__":
    fetch_and_save()