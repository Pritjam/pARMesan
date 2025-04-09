// Stores the current register file state, PSW, and all memory changes at this time.
let currentState = {};

// periodic snapshots of processor state.
let checkpoints = {};
let currentCycle = -1;
let registerNames = [];

let atBeginning = true;
let atEnd = false;

let traceDataView;

const traceFileInput = document.getElementById("traceFileInput");
const stateDisplay = document.getElementById("state-display");
const prevBtn = document.getElementById("prevBtn");
const nextBtn = document.getElementById("nextBtn");
const cycleCounter = document.getElementById("cycleCounter");
const errorMessage = document.getElementById("error-message");

const checkpointInterval = 5;

traceFileInput.addEventListener("change", (e) => {
    const file = e.target.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = (event) => {
        console.log("reading file");
        const buffer = event.target.result;
        traceDataView = new DataView(buffer);
        openTraceFile();
        updateControls();
        // TODO: allow user to reopen file without having to reload page
    };
    reader.readAsArrayBuffer(file);
});

// listeners for next and prev buttons
// TODO: more fast forward options, or a cycle number input option
prevBtn.addEventListener("click", () => {
    atEnd = false;
    if (currentCycle === -1) {
        atBeginning = true;
    } else {
        traceSeek(currentCycle - 1);
    }
    updateStateDisplay();
    updateControls();
});

nextBtn.addEventListener("click", () => {
    atBeginning = false;
    if (!atEnd) {
        traceSeek(currentCycle + 1);
    }
    updateStateDisplay();
    updateControls();
});

// read the 4-byte magic string at the beginning of a vct file.
// Returns the magic string read from the file.
function readMagic() {
    let magic = "";
    for (let i = 0; i < 4; i++) {
        magic += String.fromCharCode(traceDataView.getUint8(i));
    }
    currentState.fileOffset = 4;
    return magic;
}

// read a null-terminated string from a vct file. Pretty much only used in reading register names.
// Returns the string read from the file.
function readString() {
    let string = "";
    let char = traceDataView.getUint8(currentState.fileOffset++);
    while (char !== 0) {
        string += String.fromCharCode(char);
        char = traceDataView.getUint8(currentState.fileOffset++);
    }
    return string;
}

// Load the initial state of the processor.
// Initializes all internal data structures to track processor state.
// Also sets first checkpoint.
function loadInitialState(regNames) {
    // 16 bit initial PSW
    currentState.psw = traceDataView.getUint16(currentState.fileOffset, true);
    currentState.fileOffset += 2;
    currentState.registers = {};
    for (const regName of regNames) {
        currentState.registers[regName] = traceDataView.getUint16(currentState.fileOffset, true);
        currentState.fileOffset += 2;
    }

    currentState.memory = {};
    checkpoints[-1] = JSON.parse(JSON.stringify(currentState));
    stateDisplay.textContent = "Initial Processor State"
}

// Open and parse a trace file.
// Doesn't actually read any traces--these are done on-demand.
// Reads metadata, initializes data structures.
function openTraceFile() {
    // magic identifier
    const magicIdentifier = readMagic();
    if (magicIdentifier !== "vct!") {
        showError("Not a valid VCT file.");
        return [];
    }
    const version = traceDataView.getUint8(currentState.fileOffset++);
    console.log("Reading a vct file of version " + version.toString());
    const numOfRegisters = traceDataView.getUint8(currentState.fileOffset++);
    console.log("number of registers: ", numOfRegisters.toString());

    // skip 2 byte unused field
    currentState.fileOffset += 2;
    console.assert(currentState.fileOffset == 8, "Error: offset isn't 8 after reading the VCT header!")
    let regNames = [];
    for (let i = 0; i < numOfRegisters; i++) {
        let regName = readString();
        regNames.push(regName);
    }

    console.log("Registers we are tracking: ", regNames);
    registerNames = regNames;

    // load initial state. This also makes a checkpoint.
    loadInitialState(regNames);
    updateStateDisplay();
    console.log(currentState);

    return;
}

// read one Cycle Header and Cycle Log from the dataview and update current state.
// Return true on success, false if any error occurs.
function advanceTrace() {
    if (currentState.fileOffset >= traceDataView.byteLength) {
        return false;
    }
    currentCycle++;
    // read and parse cycle log
    const cycleLog = traceDataView.getUint8(currentState.fileOffset++);
    let memChanges = cycleLog & 0xF;
    let regChanges = (cycleLog >> 4) & 0x7;
    const pswChanged = (cycleLog >> 7) & 0x1;

    // psw
    if (pswChanged) {
        currentState.psw = traceDataView.getUint16(currentState.fileOffset, true);
        currentState.fileOffset += 2;
    }

    // register changes
    while (regChanges > 0) {
        regChanges--;
        const index = traceDataView.getUint8(currentState.fileOffset++);
        const value = traceDataView.getUint16(currentState.fileOffset, true);
        currentState.fileOffset += 2;
        currentState.registers[registerNames[index]] = value;
    }

    // memory changes
    while (memChanges > 0) {
        memChanges--;
        const address = traceDataView.getUint16(currentState.fileOffset, true);
        currentState.fileOffset += 2;
        const value = traceDataView.getUint8(currentState.fileOffset++);
        currentState.memory[address] = value;
    }

    if ((currentCycle % checkpointInterval) == 0 && !(currentCycle in checkpoints)) {
        checkpoints[currentCycle] = JSON.parse(JSON.stringify(currentState));
    }
    return true;
}

// Seek to the target cycle. When the nextBtn and prevBtn are pressed, this is called.
// Seeking backwards takes you back to a checkpoint, then seeks forward.
function traceSeek(targetCycle) {
    if (targetCycle == currentCycle) {
        return;
    } else if (targetCycle < currentCycle) {
        // we need to go backwards to the latest checkpoint before the target, then seek forwards
        var checkpointCycle = Math.max.apply(Math, Object.keys(checkpoints).filter(function (x) { return x <= targetCycle }));
        currentCycle = checkpointCycle;
        currentState = JSON.parse(JSON.stringify(checkpoints[checkpointCycle]));
        console.log("set cycle to ", currentCycle);
    }
    // check retval of advanceTrace, if it was null then we can't go forward any more
    while (targetCycle > currentCycle) {
        if (!advanceTrace()) {
            atEnd = true;
            return;
        }
    }
}


function showError(message) {
    errorMessage.textContent = `Error: ${message}`;
}


// Helper to unpack the PSW as formatted in the VCT. 
// This format is specific to pARMesan--if adapted for a different architecture,
//   this code will probably need to be changed.
const statusCodes = ["OK", "INTERRUPT", "EXCEPTION", "HALT", "ERR"];
function unpackPSW(psw) {
    ret = {};
    ret.status = statusCodes[psw & 0xFF];
    ret.flags = {};
    ret.flags['V'] = !!((psw >> 8) & 0x1);
    ret.flags['C'] = !!((psw >> 9) & 0x1);
    ret.flags['Z'] = !!((psw >> 10) & 0x1);
    ret.flags['N'] = !!((psw >> 11) & 0x1);
    ret.flags['I'] = !!((psw >> 12) & 0x1);
    ret.flags['T'] = !!((psw >> 13) & 0x1);
    return ret;
}


// Display the cycle status in the box. 
function updateStateDisplay() {
    let output = `State after execution of Cycle: ${currentCycle}\n`;
    psw = unpackPSW(currentState.psw);

    output += '\nProcessor Status:';
    output += `  ${psw.status}\n`

    output += `\nFlags:\n`;
    for (const [flag, val] of Object.entries(psw.flags || {})) {
        output += `  ${flag}: ${val}\n`;
    }

    output += `\nRegisters:\n`;
    for (const [reg, val] of Object.entries(currentState.registers || {})) {
        output += `  ${reg}: 0x${val.toString(16)}\n`;
    }

    // TODO: Update memory display?

    stateDisplay.textContent = output;
}

// Update buttons as needed, if we're at the beginning or end of the trace.
function updateControls() {
    prevBtn.disabled = atBeginning;
    nextBtn.disabled = atEnd;

    cycleCounter.textContent = `Processor State After Cycle ${currentCycle}`;
    if (atEnd) {
        errorMessage.textContent = "No further cycles to display.";
    } else if (atBeginning) {
        errorMessage.textContent = "No earlier cycles to display.";
    } else {
        errorMessage.textContent = "";
    }
}