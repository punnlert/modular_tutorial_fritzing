# BlinkBoard Node.js Backend

This is a Node.js backend that communicates with the Qt-based BlinkBoard frontend using process-based communication via stdin/stdout.

## Setup

1. **Install Node.js** (version 14 or higher)
2. **Install dependencies:**
   ```bash
   cd node-backend
   npm install
   ```

## Usage

### Start the backend:

```bash
npm start
```

### Development mode (with auto-restart):

```bash
npm run dev
```

## Communication Protocol

The backend communicates with the Qt frontend using JSON messages over stdin/stdout:

### Commands from Qt to Node.js:

```json
{"command": "init", "timestamp": 1234567890}
{"command": "ping", "timestamp": 1234567890}
{"command": "getStatus", "timestamp": 1234567890}
```

### Responses from Node.js to Qt:

```json
{"status": "success", "message": "Backend initialized successfully", "timestamp": 1234567890, "version": "1.0.0"}
{"status": "success", "message": "pong", "timestamp": 1234567890}
```

## Available Commands

- **`init`** - Initialize the backend connection
- **`ping`** - Test connectivity (returns "pong")
- **`getStatus`** - Get backend status information
- **`fileOperation`** - Example of a more complex operation

## Integration with Qt

In your Qt application, update the path in `blinkboardwidget.cpp`:

```cpp
void BlinkBoardWidget::startNodeBackend() {
  QString nodePath = "node"; // or full path to node executable
  QStringList arguments;
  arguments << "path/to/your/backend/server.js"; // Update this path

  m_nodeProcess->start(nodePath, arguments);
  // ... rest of the code
}
```

## Adding New Commands

To add new commands, modify the `processCommand` function in `server.js`:

```javascript
case 'newCommand':
  return {
    status: 'success',
    message: 'New command executed',
    data: { /* your data here */ },
    timestamp: Date.now()
  };
```

## Error Handling

The backend includes comprehensive error handling:

- JSON parsing errors
- Uncaught exceptions
- Unhandled promise rejections
- Graceful shutdown on SIGINT/SIGTERM

## Development

- Use `npm run dev` for development with auto-restart
- Check console output for debugging
- The backend will automatically restart when the Qt app reconnects
