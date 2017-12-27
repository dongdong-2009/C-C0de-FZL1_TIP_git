#pragma once

#ifndef SOEEVENTVIEWER_INCLUDE_H
#define SOEEVENTVIEWER_INCLUDE_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{

class SOEEventViewer :
	public EntityData
{
public:
	SOEEventViewer(void);
	SOEEventViewer( unsigned long key );

	virtual ~SOEEventViewer(void){}

	bool areAlarmEventsColourCoded();
	std::string getDefaultExportLocation();
	bool isCloseDisabled();
	unsigned long getEventLimit();
	unsigned long getEventRefreshTime();
	unsigned long getDefaultEventVisibilityLevel();
	std::string getExportDirectory();
	virtual std::string getType();

	static std::string getStaticType();
	virtual void invalidate();
	static IEntityData* clone(unsigned long key);

private:

	// 
	// The entity type is fixed for all instances of this class.
	// 

	static const std::string ENTITY_TYPE;

	// 
	// Supported Features
	//

	static const std::string COLOUR_CODE_ALARM_EVENTS;
	static const std::string EXPORT_FILE_PATH;
	static const std::string DISABLE_CLOSE;
	static const std::string MAX_EVENTS;
	static const std::string REFRESH_TIME;
	static const std::string DEFAULT_EVENT_VISIBILITY_LEVEL;
	//TD17888 marc_bugfix
	static const std::string EXPORT_DIRECTORY;

	//
	// Disable copy constructor and assignment operator
	//

	SOEEventViewer( const SOEEventViewer& theEventViewer );
	SOEEventViewer& operator=( const SOEEventViewer& theEventViewer );
};



}






#endif