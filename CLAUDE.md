# Interstellar — CLAUDE.md

## Project Overview
A 3D space exploration game written in C++20, built with CMake + Ninja on Windows (MSVC). The game renders a procedurally-generated, seed-based universe with frustum-culled star fields, viewed through a 3D camera rig.

## Build System
- **Generator**: CMake 3.21+ with Ninja
- **Compiler**: MSVC (C++20)
- **Presets**: `dev-debug` (debug + tests) and `dev-release` (optimized + LTO)

```powershell
# Configure
cmake --preset dev-debug

# Build
cmake --build --preset dev-debug

# Run tests
ctest --preset dev-debug

# Run the game (from repo root)
.\out\build\dev-debug\bin\Interstellar.exe
```

**Build optimizations enabled by default**: unity builds (batch 8), sccache, PCH, fast debug linking (`/DEBUG:FASTLINK`), LTO in release (`/GL /LTCG`).

## Architecture

### Module layout
Each subsystem lives in its own directory with the pattern:
```
<Module>/
  include/Interstellar/<Module>/   ← public headers
  src/Interstellar/<Module>/       ← implementation
  CMakeLists.txt
```

### Core subsystems
| Module | Purpose |
|---|---|
| `Engine/` | Fixed-timestep game loop (60 Hz sim, variable render), frame timing, config, cameras |
| `ECS/` | Minimal ECS: contiguous arrays, `Entity = uint32_t`, no deletion, `Transform`/`Velocity` components |
| `Graphics/` | `IGraphics` abstract interface — `BeginFrame/EndFrame`, mesh/shader/texture/pipeline factories |
| `Renderers/` | OpenGL + GLFW backend; Vulkan infrastructure in place but not yet active |
| `Universe/` | Seed-based 3D procedural generation — `StarGenerator3D`, `SectorStreamer`, `QueryUniverseAABB3` |
| `Scenes/` | `UniverseScene` — camera rig, star renderer, input delegation, HUD state |
| `Input/` | `InputSystem` factory; `IKeyboard`/`IMouse` abstractions; GLFW concrete backend |
| `Simulation/` | `MovementSystem`, `RenderSystem2D` — basic physics/render systems |
| `Logging/` | spdlog-based structured logging |
| `Units/` | mp-units v2.4 wrapper — type-safe dimensional analysis |
| `IO/` | Abstract filesystem (`IFilesystem`), `Expected<T>`/`Result<T>` error monads |
| `Data/` | Asset/data loading, JSON element database |
| `Utils/` | JSON helpers, string utils, time utils |
| `Procedural/` | `SeededRng` (PCG32) |

### Key entry points
- **Game loop**: `Interstellar/src/main.cpp` → `Game` class in `Interstellar.hpp`
- **Engine loop**: `Engine/include/Interstellar/Engine/Engine.hpp` — template-based run loop
- **Universe scene**: `Scenes/` — `UniverseScene::update()` / `UniverseScene::render()`
- **Shaders**: `Interstellar/assets/shaders/` — `stars.vert/.frag`, `stars_points3d.vert/.frag`

### Graphics backend
OpenGL 4.6 (GLAD loader), GLFW window. `IGraphics` interface supports future swap to Vulkan/DX12/Metal without changing game code. Renderers: `StarsRenderer`, `SkyStarsRenderer`.

## Third-party dependencies (FetchContent)
| Library | Version | Use |
|---|---|---|
| fmt | 9.1.0 | String formatting |
| spdlog | 1.11.0 | Logging |
| mp-units | 2.4.0 | Physical units |
| nlohmann/json | 3.11.2 | JSON (header-only) |
| GLAD | 0.1.36 | OpenGL loader (GL 4.6 core) |
| GLM | 0.9.9.8 | Math / linear algebra |
| GLFW | 3.3.8 | Window + input |
| CLI11 | 2.3.1 | Command-line args |
| gsl-lite | 1.0.1 | GSL contracts |
| Google Test | 1.14.0 | Unit tests |

## Tests
- Framework: Google Test
- Location: `tests/EngineTests/` and `tests/AppTests/`
- Existing coverage: `Config/`, `Data/` subsystems
- Run: `ctest --preset dev-debug` or open Test Explorer in VS

## Coding conventions
- C++20 throughout (concepts, ranges, `std::expected` pattern via `IO/Expected`)
- Interface classes prefixed with `I` (`IGraphics`, `IKeyboard`, `IMesh`)
- Public headers under `include/Interstellar/<Module>/`, never expose implementation headers
- Error handling via `Expected<T>` / `Result<T>` monads (no raw exceptions for I/O)
- PCG32 (`RngPCG32`) for all procedural randomness — keep seeding deterministic
- Avoid raw owning pointers; prefer `std::unique_ptr` / `std::shared_ptr`
- No comments unless the WHY is non-obvious

## Common tasks
- **Add a new subsystem**: create `<Name>/CMakeLists.txt`, add `add_subdirectory` in root, follow `include/Interstellar/<Name>/` header layout
- **Add a shader**: place `.vert`/`.frag` in `Interstellar/assets/shaders/`, configure CMake to copy assets to build dir
- **Add a test**: add `.cpp` under `tests/EngineTests/src/Interstellar/<Module>/`, link `gtest_main`
- **Run with custom seed**: `Interstellar.exe --seed <string>`
