open Belt;

module Impl = (Editor: Sig.Editor) => {
  module Task = Task.Impl(Editor);
  module InputMethod = InputMethod.Impl(Editor);
  open! Task;
  // from Editor Command to Tasks
  let handle =
    fun
    | Command.InputMethod.Activate => [
        WithStateP(
          state =>
            if (state.inputMethod.activated) {
              InputMethod.insertBackslash(state.editor);
              InputMethod.deactivate(state.inputMethod);
              Promise.resolved([SendEventToView(InputMethod(Deactivate))]);
            } else {
              // setContext
              Editor.setContext("agdaModeTyping", true)->ignore;

              state.inputMethod.activated = true;
              // the places where the input method is activated
              let startingOffsets: array(int) =
                Editor.getCursorPositions(state.editor)
                ->Array.map(Editor.offsetAtPoint(state.editor));
              InputMethod.activate(
                state.inputMethod,
                state.editor,
                startingOffsets,
              );
              Promise.resolved([SendEventToView(InputMethod(Activate))]);
            },
        ),
      ]
    | Deactivate => [
        WithState(
          state => {
            // setContext
            Editor.setContext("agdaModeTyping", false)->ignore;

            state.inputMethod.activated = false;
            InputMethod.deactivate(state.inputMethod);
          },
        ),
        SendEventToView(InputMethod(Deactivate)),
      ]

    | Update(sequence, translation, index) => [
        SendEventToView(InputMethod(Update(sequence, translation, index))),
      ]
    | InsertChar(char) => [
        WithState(state => {InputMethod.insertChar(state.editor, char)}),
      ]
    | ChooseSymbol(symbol) => [
        WithState(
          state => {
            InputMethod.chooseSymbol(state.inputMethod, state.editor, symbol)
          },
        ),
      ]
    | MoveUp => [
        WithState(
          state => {InputMethod.moveUp(state.inputMethod, state.editor)},
        ),
      ]
    | MoveRight => [
        WithState(
          state => {InputMethod.moveRight(state.inputMethod, state.editor)},
        ),
      ]
    | MoveDown => [
        WithState(
          state => {InputMethod.moveDown(state.inputMethod, state.editor)},
        ),
      ]
    | MoveLeft => [
        WithState(
          state => {InputMethod.moveLeft(state.inputMethod, state.editor)},
        ),
      ];
};