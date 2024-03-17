package com.robotraconteur;

public class UserLogRecordHandler extends UserLogRecordHandlerBase
{
    class UserLogRecordHandlerDirectorJava extends UserLogRecordHandlerDirector
    {
        public UserLogRecordHandlerDirectorJava(Action1<RRLogRecord> handler)
        {
            this.handler = handler;
        }

        Action1<RRLogRecord> handler;
        @Override public void handleLogRecord(RRLogRecord record)
        {
            handler.action(record);
        }
    }
    public UserLogRecordHandler(Action1<RRLogRecord> handler)
    {
        UserLogRecordHandlerDirectorJava director = new UserLogRecordHandlerDirectorJava(handler);
        int id = RRObjectHeap.addObject(director);
        _SetHandler(director, id);
    }
}
