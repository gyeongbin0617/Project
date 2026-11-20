# 📐 숫자 탐험대 (Number Explorers) 시스템 아키텍처 및 설계 명세

> 본 문서는 C 언어 및 SDL2(`SDL2`, `SDL2_image`, `SDL2_ttf`) 기반의 모듈화 구조(`include/`, `src/`)를 설계 수준에서 추상화하여 정리한 아키텍처 명세입니다.

---

## 1. 시스템 다이어그램

* **컨텍스트 다이어그램**: [CONTEXT_DIAGRAM.png](./diagrams/CONTEXT_DIAGRAM.png)
* **유스케이스 다이어그램 (`UC-01` ~ `UC-10`)**: [USE_CASE_DIAGRAM.png](./diagrams/USE_CASE_DIAGRAM.png)
* **시퀀스 다이어그램**: [SEQUENCE_DIAGRAM.png](./diagrams/SEQUENCE_DIAGRAM.png)
* **클래스 다이어그램**: [CLASS_DIAGRAM.png](./diagrams/CLASS_DIAGRAM.png)
* **패키지 다이어그램**: [PACKAGE_DIAGRAM.png](./diagrams/PACKAGE_DIAGRAM.png)

*(이미지 파일을 `docs/diagrams/` 경로에 배치하면 GitHub에서 바로 미리보기로 표시됩니다.)*

---

## 2. 클래스(모듈·구조체) 목록

| 클래스명 | 클래스 역할 | 관련 소스/헤더 파일 | 연결 유스케이스 |
| :--- | :--- | :--- | :--- |
| **`AppController`** | 애플리케이션 전체 상태(`scene`, `game`, `save`, `help_open` 등)를 총괄 관리하고 메인 업데이트·렌더링 루프를 구동합니다. | `app.h`, `app_core.h`, `app_loop.c` | 전체 흐름 제어 |
| **`InputManager`** | 키보드·마우스·휠·창 포커스 이벤트를 수신하고 씬별 입력 핸들러 및 포커스 이동(`FocusNavSpec`)을 제어합니다. | `app_input.h`, `app_input.c` | 입력 처리 전반 |
| **`HelpOverlay`** | `[F1]` 그림 기반 튜토리얼/도움말 화면의 열기·닫기 토글, 입력 일시 정지·복원(`InputSnapshot`), 도움말 화면 렌더링을 담당합니다. | `app_help.h`, `app_loop.c`, `app_input.c` | `UC-02` |
| **`GameEngine`** | 369·사칙연산·수열 등 미니게임 진행 단계(`Phase`), 난이도·서바이벌·제한 시간 규칙 및 정답/오답 판정을 처리합니다. | `game.h`, `puzzles.h`, `train.h`, `learn.h`, `app_loop.c` | `UC-01` |
| **`MistakeReview`** | 오답 발생 시 오답 기록(`MistakeNote`)을 누적하고, 오답노트 조회·재풀이 및 시각적 오답 교정 가이드를 제공합니다. | `mistakes.h`, `save.c` | `UC-03`, `UC-04` |
| **`RewardShopSettings`** | 연속 출석 보상, 코인 획득·소비, 캐릭터 스킨 및 배경 테마 구매·장착, 유저 프로필 조회 및 환경 설정(사운드·화면 등)을 관리합니다. | `shop_settings.h`, `save.c`, `app_input.c` | `UC-05` ~ `UC-10` |
| **`SaveRepository`** | 코인, 해금 스킨/배경, 출석 기록, 오답노트, 프로필 통계 및 설정 값을 로컬 파일(`save.dat`)에 저장·검증·복원합니다. | `save.c`, `app_types.h` | `UC-05` ~ `UC-10` |
| **`UIRenderer`** | 배경·헤더·패널(`UIBoxSpec`)·버튼·텍스트·토스트 배너를 렌더링하고 캐릭터와 UI 요소 간 시각적 시인성을 유지합니다. | `app_loop.c`, `scene_internal.h`, `i18n.h` | 화면·시인성 출력 |
