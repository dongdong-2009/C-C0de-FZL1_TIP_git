/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: $
  * @author 
  * @version $Revision:  $
  * Last modification: $DateTime: $
  * Last modified by:  $Author:  $
  * 
  * This Class is used to retrive the assembly information of the underlying .NET application,
  * which is used to display the Version information.
  */

#pragma once

using namespace System;
using namespace System::Reflection;

namespace NET_ISCS_Bridge 
{
	public ref class AssemblyInfoHelper
	{
	public:
		AssemblyInfoHelper(Type^ mainFrm)
		{
			assemblyType = mainFrm;
		}

		property String^ AsmName
		{
			String^  get ()
			{
				return assemblyType->Assembly->GetName()->Name->ToString(); 
			}
		}
		property String^ AsmFQName
		{
			String^  get ()
			{
				return assemblyType->Assembly->GetName()->FullName->ToString(); 
			}
		}
		property String^ CodeBase
		{
			String^  get ()
			{
				return assemblyType->Assembly->CodeBase; 
			}
		}
		property	 String^ Copyright
		{
			String^  get ()
			{
				Type^ at = AssemblyCopyrightAttribute::typeid;
				array<Object^>^ attrObjects = assemblyType->Assembly->GetCustomAttributes(at, false);
				AssemblyCopyrightAttribute^ ct = static_cast<AssemblyCopyrightAttribute^>(attrObjects[0]);
				return ct->Copyright;
			}
		}
		property String^ Company
		{
			String^  get ()
			{
				Type^ at = AssemblyCompanyAttribute::typeid;
				array<Object^>^ attrObjects = assemblyType->Assembly->GetCustomAttributes(at, false);
				AssemblyCompanyAttribute^ ct = static_cast<AssemblyCompanyAttribute^>(attrObjects[0]);
				return ct->Company;
			}
		}
		property String^ Product
		{
			String^  get ()
			{
				Type^ at = AssemblyProductAttribute::typeid;
				array<Object^>^ attrObjects = assemblyType->Assembly->GetCustomAttributes(at, false);
				AssemblyProductAttribute^ ct = static_cast<AssemblyProductAttribute^>(attrObjects[0]);
				return ct->Product;
			}
		}
		property String^ Title
		{
			String^  get ()
			{
				Type^ at = AssemblyTitleAttribute::typeid;
				array<Object^>^ attrObjects = assemblyType->Assembly->GetCustomAttributes(at, false);
				AssemblyTitleAttribute^ ct = static_cast<AssemblyTitleAttribute^>(attrObjects[0]);
				return ct->Title;
			}
		}
		property String^ Version
		{
			String^  get ()
			{
				return assemblyType->Assembly->GetName()->Version->ToString(); 
			}
		}

		property DateTime^ BuildDate
		{
			DateTime^ get()
			{
				System::IO::FileInfo^ info = gcnew System::IO::FileInfo(assemblyType->Assembly->Location);
				return info->LastWriteTime;
			}
		}

	private:
		Type^ assemblyType;

	};
}