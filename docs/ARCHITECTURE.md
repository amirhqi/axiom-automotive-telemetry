# Architecture

## Design objective

AXIOM separates transport I/O, application framing, parsing, presentation state and persistence.

`mermaid
flowchart TB
    subgraph GUI["GUI thread"]
        UI[QML UI]
        Controller[TelemetryController]
        Logger[CsvLogger]
    end

    subgraph IO["Worker I/O thread"]
        Worker[TelemetryWorker]
        Buffer[Persistent receive buffer]
    end

    Transport[TCP / UDP / Serial] --> Worker
    Worker --> Buffer
    Buffer --> Parser[PacketParser]
    Parser --> Controller
    Controller --> UI
    Controller --> Logger
`

## Thread model

Qt GUI objects stay on the GUI thread. Transport work is delegated to a worker object with worker-thread
affinity. Signals/slots bridge the thread boundary.

The purpose is not specifically "working with a board"; it is keeping asynchronous I/O and connection
handling away from the rendering thread whether data comes from a simulator, ECU, MCU, gateway, or serial device.

## TCP framing

TCP is a byte stream and does not preserve application-message boundaries. A persistent receive buffer is
therefore used until newline-delimited application frames are complete. This handles partial and coalesced reads.

## Bounded state

Trace/history data is bounded to prevent a long-running diagnostic session from growing memory without limit.

## Failure handling

Malformed frames are rejected rather than propagated into the presentation model. Connection loss is exposed
through state and, when enabled, reconnect attempts use bounded backoff.