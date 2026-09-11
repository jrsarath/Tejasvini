# Tejasvini Shared Library

This directory contains the shared types, error definitions, protocol packet specifications, and serialization utilities utilized identically by both **Tejasvini_UI** and **Tejasvini_Controller**.

## Shared Components
* **`Types.h` / `Types.cpp`**: Core state enums (`MachineState`, `ReflowProfile`, `ProfileStage`, `FanMode`), profile configurations, and two-way string mapping.
* **`ErrorCodes.h` / `ErrorCodes.cpp`**: Fault codes (`FaultCode`), severity ratings (`FaultSeverity`), and lookup helpers.
* **`ProtocolVersion.h`**: Protocol specification versioning identifiers.
* **`Config.h`**: System-wide configuration limits, default baud rates, safety thresholds, and timing parameters.
* **`Protocol.h` / `Protocol.cpp`**: High-level command, response, status, and fault framing abstractions and parsers.
* **`Serialization.h` / `Serialization.cpp`**: Wire-level text tokenizers, line formatters, and key-value extractors.

## Architectural Rule
Do **not** duplicate these files across firmware targets. Both `Tejasvini_UI` and `Tejasvini_Controller` reference this single shared directory directly or via include search paths.
