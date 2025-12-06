# ToyServer

## 개요

`ToyServer`는 플랫폼 독립적인 C++ 프로젝트로, CMake를 사용하여 빌드합니다.
이 문서는 **Windows**와 **Linux** 환경 각각에서 프로젝트를 컴파일하고 실행하는 방법을 설명합니다.

---

## 1. Windows 환경

이 가이드는 **Visual Studio (MSVC)** 가 이미 설치되어 있다고 가정합니다.
CMake가 설치되어 있지 않다면 아래 순서대로 진행해 주세요.

### 1.1 도구 설치 (CMake)

1. **CMake 다운로드**: [CMake 공식 홈페이지](https://cmake.org/download/)에서 **Windows x64 Installer**를 다운로드합니다.
2. **설치 진행**: 설치 프로그램 실행 시, **"Add CMake to the system PATH for all users"** 옵션을 반드시 체크해주세요.
3. **설치 확인**:
   새 명령 프롬프트(CMD) 창을 열고 아래 명령어를 입력하여 버전이 출력되는지 확인합니다.
   ```cmd
   cmake --version
   ```

### 1.2 프로젝트 빌드

1. **명령 프롬프트 실행**: `Win + R`을 누르고 `cmd`를 입력하거나, **Developer Command Prompt for VS**를 실행합니다.
2. **프로젝트 폴더로 이동**:
   ```cmd
   cd C:\Project\ToyServer
   ```
3. **빌드 설정 (CMake)**:
   ```cmd
   mkdir build
   cd build
   cmake ..
   ```
   *   MSVC가 설치되어 있다면 CMake가 자동으로 이를 감지하여 Visual Studio 솔루션 파일을 생성합니다.
4. **컴파일**:
   ```cmd
   cmake --build . --config Release
   ```
   *   디버그 모드로 빌드하려면 `--config Debug`를 사용하세요.
5. **실행**:
   ```cmd
   cd Release
   ToyServer.exe
   ```

---

## 2. Linux 환경

Linux 환경에서는 GCC(또는 Clang)와 CMake를 패키지 매니저를 통해 쉽게 설치할 수 있습니다.

### 2.1 도구 설치 (Ubuntu/Debian 기준)

터미널을 열고 다음 명령어를 입력하여 필수 빌드 도구를 설치합니다.

```bash
sudo apt update
sudo apt install build-essential cmake
```
*(CentOS/RHEL 계열은 `yum install gcc-c++ cmake make` 등을 사용)*

### 2.2 프로젝트 빌드

1. **프로젝트 폴더로 이동**:
   ```bash
   cd ~/Project/ToyServer
   ```
2. **빌드 설정**:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```
3. **컴파일**:
   ```bash
   cmake --build .
   ```
4. **실행**:
   ```bash
   ./ToyServer
   ```

---

## 3. 정리 (Clean Up)

빌드 과정에서 생성된 파일을 모두 삭제하고 싶다면 `build` 폴더를 삭제하세요.

**Windows:**
```cmd
cd C:\Project\ToyServer
rmdir /s /q build
```

**Linux:**

---

## 4. 힙 프로파일링 (Heap Profiling)

`tcmalloc`이 통합되어 있어 메모리 할당 분석이 가능합니다. 프로파일링을 활성화하려면 환경 변수 `HEAPPROFILE`을 설정하고 서버를 실행하세요.

### Windows (PowerShell)
```powershell
$env:HEAPPROFILE = "./heapprof"
./ToyServer.exe
```

### Linux
```bash
env HEAPPROFILE=./heapprof ./ToyServer
```

서버 실행 중 주기적으로 `heapprof.0001.heap`, `heapprof.0002.heap` 등의 파일이 생성됩니다. 이 파일들은 `pprof` 도구를 사용하여 분석할 수 있습니다.
