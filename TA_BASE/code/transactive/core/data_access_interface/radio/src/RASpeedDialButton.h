 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASpeedDialButton.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RASpeedDialButton is an implementation of IRASpeedDialButton. It holds the data specific to an RASpeedDialButton entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(RA_SPEED_DIAL_BUTTON_XX_INCLUDED_)
#define RA_SPEED_DIAL_BUTTON_XX_INCLUDED_

#include <string>
#include <algorithm>
#include <functional>

#include "core/data_access_interface/radio/src/IRASpeedDialButton.h"

namespace TA_Base_Core
{
    class IDatabase;

    class RASpeedDialButton : public IRASpeedDialButton
    {

    public:

        /**
         * RASpeedDialButton (constructor)
         *
         * Construct an RASpeedDialButton class based around a key.
         *
         * @param key The key of this RASpeedDialButton in the database
         */
        RASpeedDialButton(const unsigned long key);

        RASpeedDialButton(
            unsigned int key,
            unsigned int setKey,
            unsigned int buttonPosition,
            const std::string& buttonLabel,
            const std::string& buttonTSI,
            bool isNew
            );

        virtual ~RASpeedDialButton();

        /** 
         * Function object adapter conforming to a stl Predicate
         */
        class compareByButtonPosition : public std::unary_function<IRASpeedDialButton*, bool > 
        {
        public:
            compareByButtonPosition(const unsigned long pos) : m_pos(pos) {};
            result_type operator() (argument_type x) { return x->getButtonPosition() == m_pos; };
        private:
            const unsigned long m_pos;
        };

    public:
        
        virtual unsigned long getKey(bool reloadFlag = true);
        
        virtual unsigned long getSpeedDialSetKey();
        virtual IRASpeedDialButton& setSpeedDialSetKey(unsigned long key);

        virtual unsigned long getButtonPosition();
        virtual IRASpeedDialButton& setButtonPosition(unsigned long buttonPosition);
		
        virtual std::string getButtonLabel();
        virtual IRASpeedDialButton& setButtonLabel(const std::string& buttonLabel);
		
        virtual std::string getButtonTSI();
        virtual IRASpeedDialButton& setButtonTSI(const std::string& buttonTSI);

        virtual void invalidate(); // IItem

		virtual void applyChanges();

    private:

        // Assignment RASpeedDialButton not used so it is made private
		RASpeedDialButton& operator=(const RASpeedDialButton &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the RASpeedDialButtonHelper
        // has only been written for ConfigLocation objects and will not copy the RASpeedDialButton helper
        // for a read-only object (ie this one).
        RASpeedDialButton( const RASpeedDialButton& theRASpeedDialButton);  

        /**                                                                                 
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. OPERATORKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        void reload();

        /**
         * getNextKey()
         *
         * Get the next primary key value from the table's associated sequence. 
         */
        unsigned long getNextKey( IDatabase* databaseConnection );

        unsigned long m_key;
        unsigned long m_dialSetKey;
        unsigned long m_buttonPosition;
        std::string m_buttonLabel;
        std::string m_buttonTSI;

        bool m_isValidData;
		bool m_isWritten;
    };

} // closes TA_Base_Core


#endif // !defined(RA_SPEED_DIAL_BUTTON_XX_INCLUDED_)
