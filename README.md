# Snakegame_cpp
Snake Game README
[게임 규칙]
- 게임룰 1
Snake는 진행 방향의 반대로 이동할 수 없다.
Head 방향 이동은 무시된다.
Tail 방향으로 이동할 수 없으며, 이동 시 Game Over된다.
Snake는 자신의 Body를 통과할 수 없다.
벽(Wall)을 통과할 수 없다.
- 게임룰 2
Snake의 이동 방향에 Growth Item이 있으면 몸의 길이가 1 증가한다.
Poison Item이 있으면 몸의 길이가 1 감소하며, 길이가 3보다 작아지면 실패한다.
Item은 Snake Body가 없는 임의의 위치에 출현하며, 일정 시간 후 사라진 후 다른 위치에 나타난다. 최대 3개의 Item이 동시에 출현 가능하다.
- 게임룰 3
Gate는 두 개가 한 쌍이며, 겹치지 않는다.
Gate는 임의의 벽에 나타난다.
Snake가 Gate에 진입하면 다른 Gate로 나가게 된다.
- 게임룰 4
Gate가 나타나는 벽이 가장자리에 있을 경우, Map의 안쪽 방향으로 진출한다.
가운데에 있을 때 진출 방향은 진입 방향, 시계 및 반시계방향, 반대 방향 중 결정된다.
- 게임룰 5
Wall은 Snake가 통과할 수 없으며, Gate로 변할 수 있다.
Immune Wall은 Gate로 변할 수 없다.
Snake가 Wall 또는 Immune Wall과 충돌하면 실패한다.
- 게임룰 6
점수는 다음과 같이 계산된다:
B: Snake 몸의 최대 길이 대비 현재 길이의 비율
+: 획득한 Growth Item 수
-: 획득한 Poison Item 수
G: Gate 사용 횟수
- Map 설계
Map은 21x21 이상의 크기로 설계한다.
벽, Immune Wall, Snake Head/Body, Item, Gate 등의 요소를 표현하기 위한 2차원 배열로 구성한다.

[게임 진행]
- 단계별 진행
- Map 작성: NCurses 라이브러리를 사용하여 Snake Map을 게임 화면으로 표시한다.
- Snake 표시 및 제어: Map 위에 Snake를 표시하고 방향키 입력을 받아 Snake를 제어한다.
- Item 추가: Growth Item과 Poison Item을 Map에 추가하고 Snake와의 상호작용을 구현한다.
- Gate 추가: Wall의 임의 위치에 Gate를 추가하여 Snake의 이동을 변경한다.
- 점수 및 미션 추가: 게임 점수와 미션을 표시하고, 미션 달성 시 다음 Map으로 진행한다.


[설치]
1.   Visual Studio 2022설치
Visual Studio 공식 사이트에서 Visual Studio 2022 Community 버전을 다운로드하고 설치한다. Community 버전은 개인 및 오픈 소스 개발자에게 무료로 제공된다.
2.   PDCurses 다운로드 및 설치
PDCurses는 Window에서 NCurses 호환 라이브러리로, C 및 C++ 프로그램에서 콘솔 기반 UI를 구현하는 데에 사용된다. 
먼저 PDCurses Github 레포지토리에서 최신 소스 코드를 다운로드한다. Visual Studio를 열고 새로운 프로젝트를 생성한다. 솔루션 탐색기에서 프로젝트를 마우스 오른쪽 버튼 클릭하여 속성을 선택한다. 구성 속성 – C/C++ - 일반에서 추가 포함 디렉터리에 PDCurses 헤더 파일이 위치한 디렉터리 경로를 추가한다. 구성 속성 – 링커 – 일반에서 추가 라이브러리 디렉터리에 PDCurses 라이브러리 파일이 있는 디렉터리 경로를 추가한다. 구성 속성 – 링커 – 입력에서 추가 종속성에 pdcurses.lib를 추가한다. 프로젝트 소스 코드에서 PDCurses 헤더 파일을 include 하고, PDCurses 함수를 사용하여 콘솔 UI를 구현한다.
3.   프로그램 컴파일 및 실행
이 후 프로그램을 컴파일하고 실행한다. 컴파일 및 실행하는 방법은 다음과 같다. Visual Studio에서 PDCurses 함수를 사용하여 Snake 게임의 로직을 구현한다. Visual Studio에서 빌드 – 솔루션 빌드를 사용하여 프로젝트를 빌드한다. 빌드된 실행 파일을 실행하거나, Visual Studio에서 디버깅 모드에서 실행을 하면 된다. 
