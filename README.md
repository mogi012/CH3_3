# CH3_3 - Wave Survival (UE 5.6, C++)

한 레벨에서 3단계 웨이브가 진행되는 아이템 수집 게임. Blank C++ 프로젝트에서 시작해 게임 프레임워크와 UI 로직을 전부 C++로 구현했다.

## 구조
- `SpartaGameInstance` 누적 점수 / `SpartaGameMode` 규칙 / `SpartaGameState` 웨이브·타이머 / `SpartaPlayerController` HUD 생성
- `BaseItem` + `ItemInterface` 를 상속한 `CoinItem` `MineItem` `HealingItem`
- `SpawnVolume` 이 `WaveRow` `ItemSpawnRow` 데이터 테이블을 읽어 웨이브별 아이템 스폰
- `SpartaHUDWidget` `SpartaMainMenuWidget` `SpartaGameOverWidget` : C++ 로직 + BindWidget, 위젯 BP는 레이아웃만

## 실행
UE 5.6 / VS 2022. `Content/Assets` 는 저장소에 없음 (별도 링크).

## 시연 영상
[(YouTube 링크)](https://youtu.be/kwkD9KRCIgU)
