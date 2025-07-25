///////////////////////////////////////////////////////////////////////////////
// FILE:          ASIHub.h
// PROJECT:       Micro-Manager
// SUBSYSTEM:     DeviceAdapters
//-----------------------------------------------------------------------------
// DESCRIPTION:   ASI serial communication class (generic "hub")
//
// COPYRIGHT:     Applied Scientific Instrumentation, Eugene OR
//
// LICENSE:       This file is distributed under the BSD license.
//
//                This file is distributed in the hope that it will be useful,
//                but WITHOUT ANY WARRANTY; without even the implied warranty
//                of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//                IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//                CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//                INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES.
//
// AUTHOR:        Jon Daniels (jon@asiimaging.com) 09/2013
//
// BASED ON:      ASIStage.h
//

#ifndef ASIHUB_H
#define ASIHUB_H

#include "ASIBase.h"
#include "MMDevice.h"
#include "DeviceBase.h"
#include "DeviceThreads.h"
#include <string>

////////////////////////////////////////////////////////////////
// *********** generic ASI comm class *************************
// implements a "hub" device with communication abilities
// also acts like a device itself
// Decided to leave this separate from TigerComm in case we
// can use it separately from TG-1000 somehow, but TigerComm
// inherits from this class.
////////////////////////////////////////////////////////////////

class ASIHub : public ASIBase<HubBase, ASIHub>
{
public:
	ASIHub();
	~ASIHub() { }

	// Communication base functions
   int ClearComPort();

   // gets the response to a command but waits a certain time for the response to come instead of looking for a terminator
   // also doesn't necessarily wait for a complete response
   int QueryCommandUnterminatedResponse(const char *command, const long timeoutMs, unsigned long reply_length);
   int QueryCommandUnterminatedResponse(const char *command, const long timeoutMs) 
   {	   return  QueryCommandUnterminatedResponse(command, timeoutMs,1);   }
   int QueryCommandUnterminatedResponse(const std::string &command, const long timeoutMs)
      { return QueryCommandUnterminatedResponse(command.c_str(), timeoutMs,1); }
   int QueryCommandUnterminatedResponse(const std::string &command, const long timeoutMs, unsigned long reply_length)
   {   return QueryCommandUnterminatedResponse(command.c_str(), timeoutMs, reply_length);   }

   int QueryCommandLongReply(const char *command, const char *replyTerminator);  // all variants call this
   int QueryCommandLongReply(const char *command) { return QueryCommandLongReply(command, g_SerialTerminatorMultiLine); }
   int QueryCommandLongReply(const std::string &command) { return QueryCommandLongReply(command.c_str(), g_SerialTerminatorMultiLine); }

   // QueryCommand gets the response (optional 2nd parameter is the response's termination string) (optional 3rd parameter is delay between sending and reading response)
   int QueryCommand(const char *command, const char *replyTerminator, const long delayMs); // all variants call this
   int QueryCommand(const char *command) { return QueryCommand(command, g_SerialTerminatorDefault, (long)0); }
   int QueryCommand(const std::string &command) { return QueryCommand(command.c_str(), g_SerialTerminatorDefault, (long)0); }
   int QueryCommand(const std::string &command, const std::string &replyTerminator) { return QueryCommand(command.c_str(), replyTerminator.c_str(), (long)0); }
   int QueryCommand(const char *command, const long delayMs) { return QueryCommand(command, g_SerialTerminatorDefault, delayMs); }
   int QueryCommand(const std::string &command, const long delayMs) { return QueryCommand(command.c_str(), g_SerialTerminatorDefault, delayMs); }
   int QueryCommand(const std::string &command, const std::string &replyTerminator, const long delayMs) { return QueryCommand(command.c_str(), replyTerminator.c_str(), delayMs); }

   // QueryCommandVerify gets the response and makes sure the first characters match expectedReplyPrefix
   int QueryCommandVerify(const char *command, const char *expectedReplyPrefix, const char *replyTerminator, const long delayMs); // all variants call this
   int QueryCommandVerify(const char *command, const char *expectedReplyPrefix)
      { return QueryCommandVerify(command, expectedReplyPrefix, g_SerialTerminatorDefault, (long)0); }
   int QueryCommandVerify(const std::string &command, const std::string &expectedReplyPrefix)
      { return QueryCommandVerify(command.c_str(), expectedReplyPrefix.c_str(), g_SerialTerminatorDefault, (long)0); }
   int QueryCommandVerify(const std::string &command, const std::string &expectedReplyPrefix, const std::string &replyTerminator)
      { return QueryCommandVerify(command.c_str(), expectedReplyPrefix.c_str(), replyTerminator.c_str(), (long)0); }
   int QueryCommandVerify(const char *command, const char *expectedReplyPrefix, const long delayMs)
      { return QueryCommandVerify(command, expectedReplyPrefix, g_SerialTerminatorDefault, delayMs); }
   int QueryCommandVerify(const std::string &command, const std::string &expectedReplyPrefix, const long delayMs)
      { return QueryCommandVerify(command.c_str(), expectedReplyPrefix.c_str(), g_SerialTerminatorDefault, delayMs); }
   int QueryCommandVerify(const std::string &command, const std::string &expectedReplyPrefix, const std::string &replyTerminator, const long delayMs)
      { return QueryCommandVerify(command.c_str(), expectedReplyPrefix.c_str(), replyTerminator.c_str(), delayMs); }

   // accessing serial commands and answers
   std::string LastSerialAnswer() const { return serialAnswer_; } // use with caution!; crashes to access something that doesn't exist!
   std::string LastSerialCommand() const { return serialCommand_; }
   char LastSerialAnswerChar() const { return serialAnswer_.back(); }
   void SetLastSerialAnswer(const std::string &s) { serialAnswer_ = s; }  // used to parse subsets of full answer for commands like PZINFO using "Split" functions

   // Interpreting serial response
   int ParseAnswerAfterEquals(double &val);  // finds next number after equals sign and returns as float
   int ParseAnswerAfterEquals(long &val);  // finds next number after equals sign and returns as long int
   int ParseAnswerAfterEquals(unsigned int &val);  // finds next number after equals sign and returns as long int
   int ParseAnswerAfterUnderscore(unsigned int &val);  // finds next number after underscore and returns as long int
   int ParseAnswerAfterColon(double &val);  // finds next number after colon and returns as float
   int ParseAnswerAfterColon(long &val);  // finds next number after colon and returns as long int
	int ParseAnswerAfterPosition(unsigned int pos, double &val);  // finds next number after character position specified and returns as float
	int ParseAnswerAfterPosition(unsigned int pos, long &val);    // finds next number after character position specified and returns as long int
   int ParseAnswerAfterPosition(unsigned int pos, unsigned int &val);    // finds next number after character position specified and returns as unsigned int
   int ParseAnswerAfterPosition2(double &val);  // finds next number after character position 2 and returns as float
	int ParseAnswerAfterPosition2(long &val);    // finds next number after character position 2 and returns as long int
	int ParseAnswerAfterPosition2(unsigned int &val);    // finds next number after character position 2 and returns as unsigned int
   int ParseAnswerAfterPosition3(double &val);  // finds next number after character position 3 and returns as float
   int ParseAnswerAfterPosition3(long &val);    // finds next number after character position 3 and returns as long int
   int ParseAnswerAfterPosition3(unsigned int &val);    // finds next number after character position 3 and returns as unsigned int
   int GetAnswerCharAtPosition(unsigned int pos, char &val);  // returns the character at specified position, a safer version of LastSerialAnswer().at(pos)
   int GetAnswerCharAtPosition3(char &val);                   // returns the character at position 3, a safer version of LastSerialAnswer().at(3)

   std::vector<std::string> SplitAnswerOnDelim(const std::string &delim) const;  // splits answer on arbitrary delimeter list (any of included characters will split)
   std::vector<std::string> SplitAnswerOnCR() const { return SplitAnswerOnDelim("\r"); }
   std::vector<std::string> SplitAnswerOnSpace() const { return SplitAnswerOnDelim(" "); }

   // function to grab all the build info from BU X command
   int GetBuildInfo(const std::string &addressLetter, FirmwareBuild &build);

   // look to see if particular define is present
   bool IsDefinePresent(const FirmwareBuild &build, const std::string &defineToLookFor);

   // get define string from substring (e.g. the RING BUFFER define has the # of positions)
   std::string GetDefineString(const FirmwareBuild &build, const std::string &substringToLookFor);

   void RegisterPeripheral(const std::string &deviceLabel, const std::string &addressChar) {
      deviceMap_[deviceLabel] = addressChar;  // add device to lookup table
   }

   void UnRegisterPeripheral(const std::string &deviceLabel) {
      deviceMap_.erase(deviceLabel);  // remove device from lookup table
   }

   bool UpdatingSharedProperties() const { return updatingSharedProperties_; }

   int UpdateSharedProperties(const std::string &addressChar, const std::string &propName, const std::string &value);

   // action/property handlers
   int OnPort                       (MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnSerialTerminator           (MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnSerialCommand              (MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnSerialResponse             (MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnSerialCommandRepeatDuration(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnSerialCommandRepeatPeriod  (MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnSerialCommandOnlySendChanged(MM::PropertyBase* pProp, MM::ActionType eAct);

protected:
   std::string port_;         // port to use for communication

private:
	int ParseErrorReply() const;
	static std::string EscapeControlCharacters(const std::string &v);
	static std::string UnescapeControlCharacters(const std::string &v0);
	static std::vector<char> ConvertStringVector2CharVector(const std::vector<std::string> &v);
	static std::vector<int> ConvertStringVector2IntVector(const std::vector<std::string> &v);

   std::string serialAnswer_;      // the last answer received from any communication with the controller
   std::string manualSerialAnswer_; // last answer received when the SerialCommand property was used
   std::string serialCommand_;     // the last command sent, or can be set for calling commands without args
   std::string serialTerminator_;  // only used when parsing command sent via OnSerialCommand action handler
   long serialRepeatDuration_; // for how long total time the command is repeatedly sent
   long serialRepeatPeriod_;  // how often in ms the command is sent
   bool serialOnlySendChanged_;        // if true the serial command is only sent when it has changed
   MMThreadLock threadLock_;  // used to lock thread during serial transaction
   bool updatingSharedProperties_;
   std::map<std::string, std::string> deviceMap_;  // to implement properties shared between devices
        // key is the device name, value is the Tiger address (normally a single character, see note about addressChar_ in ASIPeripheralBase
};



#endif // ASIHUB_H
