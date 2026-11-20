# C 모듈화 결과

## 분리된 파일
- app_init.c : SDL 및 앱 초기화
- app_destroy.c : 앱/SDL 자원 해제
- app_message.c : 토스트 메시지 출력
- app_navigation.c : Scene 전환 및 입력 상태 초기화
- app_entry_payment.c : 게임 입장 코인 차감
- app_bot_skin.c : 봇 스킨 번호 계산
- app_attendance_reward.c : 출석 보상 코인/연속 출석 계산
- app_attendance.c : 날짜 계산 및 앱 시작 시 출석 보상 처리
- app_settings.c : 설정 초기화
- mistakes.c : 오답 데이터 추가/조회/복습 상태 관리
- review_feedback.c : 게임 중 오답 기록
- review_logic.c : 복습 답안 판정 및 진행 상태 갱신
- review_render.c : 복습 화면 렌더링
- review_internal.h : 복습 모듈 내부에서 공유하는 선택지 계산 함수
- scene_help_control.c : 도움말 열기/닫기
- scene_help_render.c : 도움말 화면 렌더링
- scenes.c : 현재 Scene에 맞는 화면 함수 호출

## 적용 방법
1. 기존 app.c, review.c, scene_help.c를 빌드 대상에서 제거합니다.
2. 새 .c 파일들을 프로젝트 빌드 대상에 추가합니다.
3. app_modules.h, review_internal.h를 새 헤더로 함께 둡니다.
4. mistakes.c, scenes.c는 같은 이름으로 교체합니다.
5. 기존 app.h, mistakes.h, scene_internal.h는 유지합니다.

## 검증 상태와 주의사항
제공된 5개 파일을 기준으로 함수 본문을 이동했고, 기존 게임 로직/UI 호출은 유지했습니다.
다만 app.h, mistakes.h, scene_internal.h 및 Makefile/CMakeLists.txt/IDE 프로젝트 설정이 제공되지 않아
전체 프로젝트 컴파일과 링크는 검증하지 못했습니다. 오류가 전혀 없다고 보증할 수는 없습니다.
새 .c 파일을 빌드 설정에 등록해야 하며, 기존 헤더의 함수 선언도 확인해야 합니다.
