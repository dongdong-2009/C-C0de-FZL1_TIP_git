#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace NET_ISCS_Bridge {

	/// <summary>
	/// Summary for VersionInfoForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class VersionInfoForm : public System::Windows::Forms::Form
	{
	public:
        
		VersionInfoForm(String^ vers, String^ bdate, String^ comp, String^ copyrg)
        {
            InitializeComponent();
            this->CmpValueLbl->Text = comp;
            this->dateValuelbl->Text = bdate;
            this->VersionValueLbl->Text = vers;
            this->CopyrgLbl->Text = copyrg;
            this->Text = "About " + comp;
        }

		void button1_Click(Object^ sender, System::EventArgs^ e)
        {
            this->Close();
        }

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~VersionInfoForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:

		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
		Button^ button1;
         System::Windows::Forms::Label^ label1;
         System::Windows::Forms::Label^ CmpValueLbl;
         System::Windows::Forms::Label^ label3;
         System::Windows::Forms::Label^ label5;
         System::Windows::Forms::Label^ dateValuelbl;
         System::Windows::Forms::Label^ VersionValueLbl;
         System::Windows::Forms::Label^ CopyrgLbl;
       // AxMSComCtl2.AxAnimation axAnimation1;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = gcnew System::ComponentModel::ComponentResourceManager(VersionInfoForm::typeid);
			this->button1 = gcnew System::Windows::Forms::Button();
            this->label1 = gcnew System::Windows::Forms::Label();
            this->CmpValueLbl = gcnew System::Windows::Forms::Label();
            this->label3 = gcnew System::Windows::Forms::Label();
            this->label5 = gcnew System::Windows::Forms::Label();
            this->dateValuelbl = gcnew System::Windows::Forms::Label();
            this->VersionValueLbl = gcnew System::Windows::Forms::Label();
            this->CopyrgLbl = gcnew System::Windows::Forms::Label();
           // this->axAnimation1 = new AxMSComCtl2.AxAnimation();
            //((System.ComponentModel.ISupportInitialize)(this.axAnimation1)).BeginInit();
            this->SuspendLayout();
            // 
            // button1
            // 
			this->button1->Location = System::Drawing::Point(188, 230);
            this->button1->Name = "button1";
            this->button1->Size = System::Drawing::Size(119, 33);
            this->button1->TabIndex = 0;
            this->button1->Text = "OK";
            this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &VersionInfoForm::button1_Click);
            // 
            // label1
            // 
			this->label1->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, ((unsigned char)(0)));
            this->label1->AutoSize = true;
            this->label1->Location = System::Drawing::Point(12, 70);
            this->label1->Name = "label1";
            this->label1->Size = System::Drawing::Size(64, 13);
            this->label1->TabIndex = 1;
            this->label1->Text = "Component:";
            // 
            // CmpValueLbl
            // 
			this->CmpValueLbl->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, ((unsigned char)(0)));
            this->CmpValueLbl->Location = System::Drawing::Point(106, 70);
            this->CmpValueLbl->Name = "CmpValueLbl";
            this->CmpValueLbl->Size = System::Drawing::Size(285, 23);
            this->CmpValueLbl->TabIndex = 2;
            this->CmpValueLbl->Text = "label2";
            // 
            // label3
            // 
			this->label3->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, ((unsigned char)(0)));
            this->label3->AutoSize = true;
            this->label3->Location = System::Drawing::Point(12, 114);
            this->label3->Name = "label3";
            this->label3->Size = System::Drawing::Size(45, 13);
            this->label3->TabIndex = 3;
            this->label3->Text = "Version:";
            // 
            // label5
            // 
			this->label5->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, ((unsigned char)(0)));
            this->label5->AutoSize = true;
            this->label5->Location =  System::Drawing::Point(12, 158);
            this->label5->Name = "label5";
            this->label5->Size =  System::Drawing::Size(59, 13);
            this->label5->TabIndex = 5;
            this->label5->Text = "Build Date:";
            // 
            // dateValuelbl
            // 
			this->dateValuelbl->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, ((unsigned char)(0)));
            this->dateValuelbl->Location =  System::Drawing::Point(106, 158);
            this->dateValuelbl->Name = "dateValuelbl";
            this->dateValuelbl->Size =  System::Drawing::Size(285, 23);
            this->dateValuelbl->TabIndex = 6;
            this->dateValuelbl->Text = "label6";
            // 
            // VersionValueLbl
            // 
			this->VersionValueLbl->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, ((unsigned char)(0)));
            this->VersionValueLbl->Location =  System::Drawing::Point(106, 114);
            this->VersionValueLbl->Name = "VersionValueLbl";
            this->VersionValueLbl->Size =  System::Drawing::Size(285, 23);
            this->VersionValueLbl->TabIndex = 4;
            this->VersionValueLbl->Text = "label4";
            // 
            // CopyrgLbl
            // 
			this->CopyrgLbl->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, ((unsigned char)(0)));
            this->CopyrgLbl->Location =  System::Drawing::Point(66, 202);
            this->CopyrgLbl->Name = "CopyrgLbl";
			this->CopyrgLbl->Size =  System::Drawing::Size(421, 23);
            this->CopyrgLbl->TabIndex = 7;
            this->CopyrgLbl->Text = "label2";
            // 
            // axAnimation1
            // 
            /*this.axAnimation1.Location = new System.Drawing.Point(125, 13);
            this.axAnimation1.Name = "axAnimation1";
            this.axAnimation1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axAnimation1.OcxState")));
            this.axAnimation1.Size = new System.Drawing.Size(266, 38);
            this.axAnimation1.TabIndex = 8;*/
            // 
            // VersionInfoForm
            // 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize =  System::Drawing::Size(499, 275);
            //this->Controls.Add(this.axAnimation1);
            this->Controls->Add(this->CopyrgLbl);
            this->Controls->Add(this->dateValuelbl);
            this->Controls->Add(this->label5);
            this->Controls->Add(this->VersionValueLbl);
            this->Controls->Add(this->label3);
            this->Controls->Add(this->CmpValueLbl);
            this->Controls->Add(this->label1);
            this->Controls->Add(this->button1);
            this->MaximizeBox = false;
            this->MinimizeBox = false;
            this->Name = "VersionInfoForm";
            this->Text = "About";
           // ((System->ComponentModel->ISupportInitialize)(this->axAnimation1))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();
        }
#pragma endregion
	};
}
