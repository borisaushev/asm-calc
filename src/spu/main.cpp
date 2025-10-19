#include "common.h"
#include "stack.h"
#include "processor.h"

int main() {
    processor_t processor = {};
    initProcessor(&processor);

    _TX_TRY {
        _TX_CHECKED (parseCommands(BYTECODE_PATH, &processor))
        _TX_CHECKED (verifyProcessor(&processor));
        _TX_CHECKED (runCommands(&processor));
    } _TX_ENDTRY
    _TX_CATCH {
    }
    _TX_FINALLY {
        destroyProcessor(&processor);
    }

    return 0;
}