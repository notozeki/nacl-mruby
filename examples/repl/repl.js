$(function () {
  var header = 'Loading module...';
  var jqconsole = $('#console').jqconsole(header, '>> ');
  var seqid = 0;
  var startPrompt = function () {
    jqconsole.Prompt(true, function (input) {
      common.naclModule.postMessage({type: 'request',
                                     id: seqid++,
                                     body: input});
    });
  };

  // Called by the common.js module.
  window.moduleDidLoad = function () {
    common.hideModule();
    jqconsole.Write('done.\nWelcome to nacl-mruby REPL console :)\n', 'jqconsole-header');
    startPrompt();
  };
  // Called by the common.js module.
  window.handleMessage = function (message) {
    if (typeof(message.data) == 'string') { /* stdout */
      jqconsole.Write(message.data, 'jqconsole-output');
      return;
    }
    switch ('' + message.data.type) { /* response message */
    case 'response':
      jqconsole.Write('=> ' + message.data.body + '\n', 'jqconsole-output');
      break;
    case 'exception':
      jqconsole.Write(message.data.body + '\n', 'jqconsole-output-exception');
      break;
    default:
      throw 'unknown type';
      break;
    }
    startPrompt();
  };
});
