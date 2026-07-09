- Prefer the platform-specific scripts in `run/` over `CMakeLists.txt`, `cmake --build`, or ad-hoc compile commands.
- Treat `CMakeLists.txt` as editor support for CLion/indexing unless the user explicitly asks to maintain or use a CMake build.
- Build and run:
  ```bash
  # Build and run
  sh run/build-linux.sh
  sh run/run-linux.sh
  ```

