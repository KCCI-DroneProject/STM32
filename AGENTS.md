# KCCI-Drone 작업 규칙

## 프로젝트
- STM32H743 드론 비행 제어 펌웨어다. C11(gnu11) / STM32 HAL / FreeRTOS(CMSIS-RTOS2) / CMake + Ninja / arm-none-eabi-gcc.
- 빌드 정의는 `CMakeLists.txt`, CubeMX 생성 빌드는 `cmake/stm32cubemx/CMakeLists.txt`를 확인한다. 필요한 파일만 읽는다.
- 사용자와의 설명 및 주석은 한국어를 기본으로 하고, 코드 식별자는 영어로 작성한다.

## 폴더 구조
| 경로 | 내용 | 수정 |
| --- | --- | --- |
| `K-Drone/` | 직접 작성하는 코드 (`App`, `Config`, `Drivers`, `Utils`) | 자유 |
| `Core/` | CubeMX 생성 코드 | `USER CODE BEGIN` ~ `END` 블록 안에서만 |
| `Drivers/`, `Middlewares/`, `startup_*.s`, `*.ld` | HAL·CMSIS·FreeRTOS·스타트업·링커 | 요청이 없으면 수정하지 않는다 |
| `KCCI-Drone.ioc` | CubeMX 설정 | 직접 편집하지 않는다 |

- `K-Drone/` 폴더는 `CMakeLists.txt`의 `KDRONE_SOURCES`(소스, `폴더/*.c` 형태), `KDRONE_INCLUDE_DIRS`(헤더) 목록에 등록한다. 등록된 소스 폴더 안의 `.c`는 자동 포함되므로 파일 추가 때는 CMake를 수정하지 않고, 새 폴더를 만들 때만 목록에 추가한다.
- 미리 컴파일할 표준 헤더(PCH)는 `CMakeLists.txt`의 `target_precompile_headers`에 추가한다. 프로젝트 헤더는 넣지 않는다.

## 코딩 컨벤션
### 이름 규칙
| 대상 | 규칙 | 예시 |
| --- | --- | --- |
| 변수·매개변수·구조체 멤버 | camelCase | `xPos`, `yPos`, `zPos` |
| 함수 | `K` + 모듈명 + `_` + PascalCase (자동완성을 위해 주체를 앞에 둔다) | `KPoint_Init()`, `KPoint_GetXPos()`, `KMotor_SetSpeed()` |
| 구조체 | PascalCase, `typedef struct` | `KPoint` |
| 상수·매크로 | 대문자 + SNAKE_CASE | `STM_VERSION`, `PI` |
| 모듈 파일 | 모듈명과 일치하는 `.h` / `.c` 짝 | `KMotor.h`, `KMotor.c` |

- 인덱스처럼 바로 의미가 보이는 숫자는 상수화하지 않는다. 의미가 드러나지 않는 숫자는 알아보기 쉬운 이름의 상수로 만든다.

### 하지 말아야 할 것
- `extern` 사용을 자제한다. 다른 모듈의 데이터는 `KPoint_GetXPos()` 같은 함수로 접근한다.
- 힙 할당(`malloc`, `free` 등)을 자제한다. 버퍼는 정적으로 할당한다.
- 헤더에 `static` 함수·변수를 선언하지 않는다. (금지)

### 하면 좋은 것
- 값이 변하지 않는 변수와 매개변수에는 `const`를 붙인다.
- 파일 내부에서만 쓰는 함수와 전역 변수에는 `static`을 붙인다.
- 헤더에는 그 헤더가 직접 쓰는 헤더만 include하고, 가능하면 필요한 헤더는 소스 파일에서 include한다. (상호 참조 방지)

### 형식
- 헤더는 `#pragma once`를 사용한다. `.c` 파일에는 넣지 않는다.
- 들여쓰기는 공백 4칸, 중괄호는 여는/닫는 중괄호를 각각 독립된 줄에 둔다. 형식은 `.clang-format`을 따른다.
- 새로 작성하거나 수정하는 코드에만 적용하고, 관련 없는 기존 코드나 CubeMX 생성 코드를 재포맷하지 않는다.

```c
#define STM_VERSION 1.0

const float PI = 3.14f;

typedef struct
{
    int xPos;
    int yPos;
    int zPos;
} KPoint;

int KPoint_GetXPos(void)
{
    return xPos;
}
```

## 최소 구현 원칙 — Ponytail 기반
공식 원칙: https://github.com/DietrichGebert/ponytail/blob/main/AGENTS.md
이 문서는 프로젝트용 요약 규칙이며 플러그인 설치나 훅·명령어 활성화를 의미하지 않는다.

- 요구사항을 완전히 만족하는 가장 작은 변경을 선택한다. 요청하지 않은 기능, 미래용 옵션, 래퍼, 추상화 계층은 추가하지 않는다.
- 기존 프로젝트 구현 → C 표준 라이브러리 → HAL·FreeRTOS·CMSIS → 필요한 최소 직접 구현 순서로 검토한다. 몇 줄을 줄이려고 의존성을 추가하지 않는다.
- 줄 수보다 가독성과 정확성을 우선한다. 난해한 한 줄 표현, 매크로 남용, 의미 없는 축약을 피한다.
- 변경 전에 호출부와 영향을 받는 설정(`Core/Inc/*.h`, `FreeRTOSConfig.h`, `.ioc`)을 검색한다. 공통 원인을 한 곳에서 고치고 필요한 연결부까지 완료한다.
- 입력 검증, 오류 처리, 타임아웃, 페일세이프(모터 정지 등), 인터럽트 안전성은 간결화를 이유로 제거하지 않는다.
- 주석은 코드로 드러나지 않는 이유를 짧게 설명한다. 알려진 임시 제약은 `shortcut: <제약>, <개선이 필요한 조건>`으로 기록한다.
- 검색은 `rg`로 범위를 좁히고, 이미 확인한 내용을 불필요하게 다시 읽지 않는다. 문서·로그·답변은 필요한 정보 위주로 유지한다.

## 펌웨어 구현 규칙
- FreeRTOS 객체는 기존 힙 설정(`heap_4`)을 따른다.
- 인터럽트 핸들러(ISR)와 콜백에서는 블로킹 호출, `HAL_Delay`, 긴 연산을 하지 않는다. FreeRTOS API는 `FromISR` 버전을 사용한다.
- 태스크에서 `HAL_Delay` 대신 `osDelay`/`vTaskDelay`를 사용한다. 태스크 간 공유 데이터는 큐·뮤텍스 등으로 보호한다.
- 핀·주변장치 설정은 CubeMX(`.ioc`)에서 바꾸는 것이 원칙이다. 코드로 바꿔야 하면 이유를 보고한다.
- 모터 출력에 영향을 주는 변경은 초기값과 오류 시 안전 상태(출력 0)를 확인한다.

## 검증 및 보고
- 코드 변경 후 `cmake --preset Debug` → `cmake --build --preset Debug`로 빌드를 확인한다. 툴체인은 STM32Cube 번들(`%LOCALAPPDATA%\stm32cube\bundles`)의 `arm-none-eabi-gcc`, `ninja`를 사용한다.
- 새 경고를 남기지 않는다. 문서만 바꾼 경우 빌드는 생략해도 된다.
- 하드웨어 동작은 직접 확인할 수 없으므로, 보드에서 확인해야 할 항목을 보고에 명시한다.
- 사용자 변경을 보존하고, 완료 전에 diff를 확인해 작업 범위 밖 변경이 없는지 살핀다.
- 결과는 변경 내용과 검증 결과를 간결하게 보고한다. 확인하지 못한 동작과 실제 남은 제약이 있으면 명시한다.
