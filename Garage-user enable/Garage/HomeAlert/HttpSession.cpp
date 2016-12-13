#include "HttpSession.h"

HttpSession::HttpSession() {
  int i;
  for (i=0; i<MAX_HTTP_SESSION; i++) {
  	delay(0);
    sessionId[i] = "";
  }
}

String HttpSession::GetRandomString() {
  MD5Builder md5;  
  md5.begin();
  md5.add(String(random(0x7fffffff)));
  md5.add(String(millis()));
  md5.calculate();
  return md5.toString();
}

long HttpSession::GetTag(const String& session) {
  int idx = Find(session);
  if ( idx>=0 && idx<MAX_HTTP_SESSION ) return sessionTag[idx];
  return 0;
}

bool HttpSession::SetTag(const String& session, long tag) {
  int idx = Find(session);
  if ( idx>=0 && idx<MAX_HTTP_SESSION ) {
    sessionTag[idx] = tag;
    return true;  
  }
  return false;
}

bool HttpSession::isExpired(int idx, time_t cur) {
  return (idx>=0 
    && idx<MAX_HTTP_SESSION
    && sessionTime[idx]<=cur
    && (cur-sessionTime[idx])>SESSION_EXPIRE_TIME);
}

bool HttpSession::isValid(const String& session, long* tag) {
  int i = Find(session);
  time_t cur = now();

  TRACE2("Find session: ",i);

  if ( i>=0 && i<MAX_HTTP_SESSION ) {
    // if there is a valid session, then update the session expired time
    // and return the tag of the session
    if (  !isExpired(i,cur) ) {
      TRACE2("Session OK. Tag = ",sessionTag[i]);
      sessionTime[i] = cur;
      if ( tag!=NULL ) *tag = sessionTag[i];
      return true;
    }
    else {
      TRACE("Session expired");
      Delete(i);
    }
  }  
  return false;
}

int HttpSession::Find(const String& session) {
  int i;
  if ( session.length()<1 ) return -1;
  for (i=0; i<MAX_HTTP_SESSION; i++) {
  	delay(0);
    if ( sessionId[i]==session ) return i;
  }
  return -1;
}

String HttpSession::Create(long tag) {
  int i;
  time_t cur = now();
  
  for (i=0; i<MAX_HTTP_SESSION; i++) {
  	delay(0);
    if ( sessionId[i].length()<1 || isExpired(i,cur) ) {
      sessionId[i] = GetRandomString();
      sessionTag[i] = tag;
      sessionTime[i] = cur;
      return sessionId[i];
    }
  }
  return String("");
}

void HttpSession::Delete(int idx) {
  if ( idx>=0 && idx<MAX_HTTP_SESSION ) {
    sessionId[idx] = "";
    sessionTime[idx] = 0;
    sessionTag[idx] = 0;
    TRACE2("Delete Session",idx);
  }  
}

void HttpSession::Delete(const String& session) {
  Delete(Find(session));
}

