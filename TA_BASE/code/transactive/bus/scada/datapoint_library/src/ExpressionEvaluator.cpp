/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/ExpressionEvaluator.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:43:23 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */

#include "app/scada/common/src/CommonDefs.h"
#include "app/scada/datapoint_library/src/IExpressionNodeNotification.h"
#include "app/scada/datapoint_library/src/ExpressionNode.h"
#include "app/scada/datapoint_library/src/DataPointValue.h"
#include "app/scada/datapoint_library/src/DataPointState.h"
#include "app/scada/datapoint_library/src/ExpressionEvaluator.h"

// static members
const char ExpressionEvaluator::EQUALITY_CHARACTER = '=';
const char ExpressionEvaluator::WHITE_SPACE_CHARACTER = ' ';
const char ExpressionEvaluator::END_OF_STRING_CHARACTER = '\0';
const char ExpressionEvaluator::OPEN_BRACKET_CHARACTER = '(';
const char ExpressionEvaluator::CLOSE_BRACKET_CHARACTER = ')';
const char ExpressionEvaluator::DOT_CHARACTER = '.';


ExpressionEvaluator::ExpressionEvaluator ( IExpressionNodeNotification * user )
:
m_expressionString ( "" ),
m_validExpression ( false ),
m_evaluationError ( false ),
m_currentPosition ( 0 ),
m_currentNode ( 0 ),
m_user ( 0 )
{
	m_user = user;
}


ExpressionEvaluator::~ExpressionEvaluator()
{
	// do not delete the reference to the client of interface IExpressionNodeNotification
	m_user = 0;

	// delete the ExpressionNode
	if ( 0 != m_currentNode )
	{
		delete m_currentNode;
		m_currentNode = 0;
	}
}


void ExpressionEvaluator::updateExpression ( const std::string & expression )
{
	// only proceed if there is a difference
	if ( 0 != expression.compare ( m_expressionString ) )
	{
		m_expressionString = expression;

		// clean up previous expression node
		if ( 0 != m_currentNode )
		{
			delete m_currentNode;
			m_currentNode = 0;
		}

		parseExpression ( m_expressionString );
	}
}


bool ExpressionEvaluator::getIsValidExpression() const
{
	return m_validExpression;
}


bool ExpressionEvaluator::getResult()
{
	double value = 0;
	
	if ( true == m_validExpression )
	{
		if ( 0 != m_currentNode )
		{
			// reset error flag
			m_evaluationError = false;
			value = evaluate ( m_currentNode );
		}
	}

	// if error flag is set
	if ( true == m_evaluationError )
	{
		return false;
	}

	// if the evaluation of the expression is successfull
	else
	{
		return ( ( 0 == value ) ? false : true );
	}	
}


void ExpressionEvaluator::incrementStrPointer()
{
	m_currentPosition++;
}


void ExpressionEvaluator::SkipSpaces()
{
	while ( ( m_expressionString [ m_currentPosition ] == WHITE_SPACE_CHARACTER ) &&
			( m_expressionString [ m_currentPosition ] != END_OF_STRING_CHARACTER ) )
	{
		incrementStrPointer();
	}
}


void ExpressionEvaluator::parseExpression ( const std::string & expression )
{
	// check if the expression passed in an empty expression
	if ( true == expression.empty() )
	{
		// do nothing
	}

	// intialise variables
	m_currentNode = 0;
	m_currentPosition = 0;
	m_validExpression = true;

	// parse the incomming expression to create an ExpressionNode object
	m_currentNode = findEquality();

	// if no expression node created
	if ( 0 == m_currentNode )
	{
		m_validExpression = false;
	}
}


ExpressionNode * ExpressionEvaluator::findEquality()
{
	ExpressionNode *  tempNode = 0;
	ExpressionNode *  left = 0;
	ExpressionNode *  right = 0;

	// the open and close brackets
	left = findOpenCloseBracket();

	if ( 0 == left ) return 0;
	
	// if the current character of the expression is the '=' character
	if ( m_expressionString[ m_currentPosition ] == EQUALITY_CHARACTER )
	{
		// create a new node to mark the '=' character
		tempNode = new ExpressionNode ( m_user );

		// assign the new node to the 
		tempNode->setLeftExpressionNode ( left );
		tempNode->setOperator ( m_expressionString [ m_currentPosition ] );

		incrementStrPointer();

		right = findEquality();

		if ( 0 == right ) return 0;

		tempNode->setRightExpressionNode ( right );
		left = tempNode;
	}

	return left;
}


ExpressionNode * ExpressionEvaluator::findOpenCloseBracket()
{
	ExpressionNode * tempNode = 0;

	SkipSpaces();

	// if current character of the expression is an open bracket
	if ( m_expressionString [ m_currentPosition ] == OPEN_BRACKET_CHARACTER )
	{
		// point to next character of the expression
		incrementStrPointer();

		// look for the '=' character
		tempNode = findEquality();

		// if there is no '=' character
		if ( 0 == tempNode ) return 0;

		// if there is no closing bracket matching the open bracket in the expression
		if ( m_expressionString [ m_currentPosition ] != CLOSE_BRACKET_CHARACTER )
		{
			return 0;
		}

		// point to next character of the expression
		incrementStrPointer();

		return tempNode;
	}

	// find the item associated with the current character
	else
	{
		return findIdentifier();
	}

	return tempNode;
}


ExpressionNode * ExpressionEvaluator::findIdentifier()
{
	ExpressionNode * nod = 0;
	ExpressionNode * nod2 = 0;
	ExpressionNode * result = 0;

	SkipSpaces();

	int position = 0;
	position = m_currentPosition;

	if ( m_expressionString [ position ] == END_OF_STRING_CHARACTER )
	{
		result = 0;
	}

	// if current charater of the expression is a digit
	if ( isdigit ( m_expressionString [ position ] ) )
	{
		// look for the end of the numerical expression
		while ( ( isdigit ( m_expressionString [ m_currentPosition ] ) ) ||
				( m_expressionString [ m_currentPosition ] == DOT_CHARACTER ) )
		{
			// point to next character of the expression
			incrementStrPointer();
		}

		nod = new ExpressionNode ( m_user );
		nod->setLeftExpressionNode ( 0 );
		nod->setRightExpressionNode ( 0 );
		nod->setOperator ( 'V' ); // indicate the node containing a value, not a mathematical operator
		nod->setNumberString ( m_expressionString.substr ( position, m_currentPosition - position ) );
		result = nod;
	}
	
	// if current character is not a digit character
	else if ( isalpha ( m_expressionString [ position ] ) )
	{
		// look for the end of the alpha numerical expression
		while ( ( isalnum ( m_expressionString [ m_currentPosition ] ) ) ||
				( ( m_expressionString [ m_currentPosition ] == DOT_CHARACTER ) ||
				  ( m_expressionString [ m_currentPosition ] == '-' ) ) )
		{
			// point to next character of the expression
			incrementStrPointer();
		}

		nod = new ExpressionNode ( m_user );
		nod->setLeftExpressionNode ( 0 );
		nod->setRightExpressionNode ( 0 );
		nod->setOperator ( 'V' ); // indicate the node containing a value, not a mathematical operator
		nod->setDataPointNameString ( m_expressionString.substr ( position, m_currentPosition - position ) );
		
		result = nod;
	}

	else
	{
		// do nothing
	}

	SkipSpaces();

	return result;
}


double ExpressionEvaluator::evaluate ( const ExpressionNode * currentNode )
{
	double result = 0;

	if ( 0 == currentNode )
	{
		m_evaluationError = true;
		return result;
	}

	switch ( currentNode->getOperator() )
	{
		// equality character
		case EQUALITY_CHARACTER:
		{
			ExpressionNode * left = 0;
			ExpressionNode * right = 0;

			left = currentNode->getLeftExpression();
			right = currentNode->getRightExpression();

			if ( ( 0 != left ) && ( 0 != right ) )
			{
				result = ( evaluate ( left ) == evaluate ( right ) );
			}
			else
			{
				m_evaluationError = true;
			}

			break;
		}

		// value character
		case 'V':
		{
			if ( 0 != currentNode )
			{
				if ( ( QUALITY_GOOD_NO_SPECIFIC_REASON == currentNode->getCurrentState().getStatus() ) ||
					 ( QUALITY_GOOD_LOCAL_OVERRIDE == currentNode->getCurrentState().getStatus() ) )
				{
					switch ( currentNode->getCurrentState().getValue().getDataPointDataType() )
					{
						case TA_Base_Bus::DataPointCorbaDef::DPT_NULL_TYPE:
						case TA_Base_Bus::DataPointCorbaDef::DPT_TEXT_TYPE:
						{
							m_evaluationError = true;
							break;
						}
						case TA_Base_Bus::DataPointCorbaDef::DPT_BOOL_TYPE:
						{
							result = ( currentNode->getCurrentState().getValue().getBoolean() == true ? 1 : 0 );
							break;
						}
						case TA_Base_Bus::DataPointCorbaDef::DPT_ENUM_TYPE:
						{
							result = (double)currentNode->getCurrentState().getValue().getEnum();
							break;
						}
						default:
						{
							result = currentNode->getCurrentState().getValue().getFloat();
							break;
						}
					}
				}
				else
				{
					m_evaluationError = true;
				}
			}
			else
			{
				m_evaluationError = true;
			}

			break;
		}
 
		default:
		{
			m_evaluationError = true;

			break;
		}
	}

	return result;
}

