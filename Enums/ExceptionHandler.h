#ifndef EXCEPTION_HANDLER_H
#define EXCEPTION_HANDLER_H

enum ExceptionHandler
{
    NoError = 0,
    UnknownParamsException = 1,
    PenguinsNumValueException = 2,
    GamePhaseValueException = 3,
    FileFormatException = 4,
    FileOpenException = 5,
    MoveImpossible = 6,
} globalExceptionHandler;

#endif