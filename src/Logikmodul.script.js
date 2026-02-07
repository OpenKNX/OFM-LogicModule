function LOG_Nop(input, output, context) { }

function LOG_ClearContent(input, output, context) {
    output.ResultDisplay = "";
}

function LOG_checkUserFormula(device, online, progress, context) {
    var lFormulaIndex = context.formulaIndex;
    var lParFormulaName = "LOG_UserFormula" + lFormulaIndex;
    progress.setText("Logik: Benutzerformel " + lFormulaIndex + " prüfen...");
    var resp = LOG_processUserFormula(1, lParFormulaName, lFormulaIndex, device, online, progress, context);
    if (resp[0] == 0) {
        progress.setText("Logik: Benutzerformel " + lFormulaIndex + " ist korrekt");
    }
}

function LOG_testUserFormula(device, online, progress, context) {
    progress.setText("Logik: Testformel berechnen...");
    var resp = LOG_processUserFormula(2, "LOG_TestFormula", 0, device, online, progress, context);
    if (resp[0] == 0) {
        // result is a 0 terminated string
        var result = "";
        for (var index = 1; resp[index] > 0 && index < 32; index++) {
            result += String.fromCharCode(resp[index]);
        }
        progress.setText("Logik: Testformel ergibt " + result);
        var parTestResult = device.getParameterByName("LOG_TestFormulaResult");
        parTestResult.value = result;
    }
}

function LOG_processUserFormula(command, parFormulaName, iFormulaIndex, device, online, progress, context) {
    // first we try to get rid of all '\n' strings
    newline(device, online, progress, { "textbox": parFormulaName })

    var parFormula = device.getParameterByName(parFormulaName);
    var lFormula = parFormula.value;
    if (lFormula.length == 0) {
        throw new Error("Logik: Formel ist leer");
    }
    online.connect();
    var data = [command]; // check user formula command

    // user formula index to check
    data = data.concat(iFormulaIndex);
    // send formula length to avoid APDU problems
    data = data.concat(lFormula.length);
    // we send the client formula, because it might not have been sent to device
    // and we convert everything to lowercase
    for (var i = 0; i < lFormula.length; i++) {
        var code = lFormula.charCodeAt(i);
        // if (code >= 65 && code <= 90) code += 32;
        data = data.concat([code]);
    }
    data = data.concat(0); // ensure zero-terminated string

    var resp = online.invokeFunctionProperty(160, 4, data);
    online.disconnect();
    if (resp[0] == 0)
        return resp;
    else if (resp[0] == -1)
        throw new Error("Logik: Fehler bei der Übertragung:\n\nAPDU ist zu kurz, prüfen der Formel nicht möglich");
    else {
        var lErr = resp[0];
        var lErrorFormula = parFormula.value;
        var lStart = lErrorFormula.substr(0, lErr - 1);
        var lLetter = lErrorFormula.charAt(lErr - 1);
        var lEnd = lErrorFormula.slice(lErr);
        throw new Error("Logik: Fehler bei '" + lLetter + "':\n\n" + lStart + " ==>" + lLetter + "<== " + lEnd);
    }
}

function LOG_CalcAbsFromRel(input, output, context) {
    if (input.AbsRel == 2) {
        output.AbsReadKO = context.OwnKO + input.RelWriteKO;
        output.AbsWriteKO = output.AbsReadKO;
    }
}

function LOG_CalcRelFromAbs(input, output, context) {
    if (output.AbsRel == 1) {
        output.RelWriteKO = input.AbsWriteKO - context.OwnKO;
    }
}

