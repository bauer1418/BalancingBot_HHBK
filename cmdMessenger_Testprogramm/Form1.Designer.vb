<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Das Formular überschreibt den Löschvorgang, um die Komponentenliste zu bereinigen.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Wird vom Windows Form-Designer benötigt.
    Private components As System.ComponentModel.IContainer

    'Hinweis: Die folgende Prozedur ist für den Windows Form-Designer erforderlich.
    'Das Bearbeiten ist mit dem Windows Form-Designer möglich.  
    'Das Bearbeiten mit dem Code-Editor ist nicht möglich.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.bttCOMVerbinden = New System.Windows.Forms.Button()
        Me.cmbCOMPort = New System.Windows.Forms.ComboBox()
        Me.cmbBefehl = New System.Windows.Forms.ComboBox()
        Me.bttBefehl_senden = New System.Windows.Forms.Button()
        Me.txtArgument = New System.Windows.Forms.TextBox()
        Me.lbbCOMStatus = New System.Windows.Forms.Label()
        Me.rbbCOMStatus = New System.Windows.Forms.RadioButton()
        Me.lbEmpfangeDaten = New System.Windows.Forms.ListBox()
        Me.lbbZyklusdaten = New System.Windows.Forms.Label()
        Me.bttMotorenEINAUS = New System.Windows.Forms.Button()
        Me.bttPID_Winkel_ändern = New System.Windows.Forms.Button()
        Me.bttPID_Speed_ändern = New System.Windows.Forms.Button()
        Me.Akku_Timer = New System.Windows.Forms.Timer(Me.components)
        Me.LbbAkku = New System.Windows.Forms.Label()
        Me.bttPIDFern = New System.Windows.Forms.Button()
        Me.bttAufnahme = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        '
        'bttCOMVerbinden
        '
        Me.bttCOMVerbinden.Location = New System.Drawing.Point(12, 51)
        Me.bttCOMVerbinden.Name = "bttCOMVerbinden"
        Me.bttCOMVerbinden.Size = New System.Drawing.Size(118, 23)
        Me.bttCOMVerbinden.TabIndex = 0
        Me.bttCOMVerbinden.Text = "Port verbinden"
        Me.bttCOMVerbinden.UseVisualStyleBackColor = True
        '
        'cmbCOMPort
        '
        Me.cmbCOMPort.FormattingEnabled = True
        Me.cmbCOMPort.Location = New System.Drawing.Point(12, 12)
        Me.cmbCOMPort.Name = "cmbCOMPort"
        Me.cmbCOMPort.Size = New System.Drawing.Size(118, 21)
        Me.cmbCOMPort.TabIndex = 1
        '
        'cmbBefehl
        '
        Me.cmbBefehl.FormattingEnabled = True
        Me.cmbBefehl.Location = New System.Drawing.Point(136, 12)
        Me.cmbBefehl.Name = "cmbBefehl"
        Me.cmbBefehl.Size = New System.Drawing.Size(224, 21)
        Me.cmbBefehl.TabIndex = 3
        '
        'bttBefehl_senden
        '
        Me.bttBefehl_senden.Location = New System.Drawing.Point(366, 12)
        Me.bttBefehl_senden.Name = "bttBefehl_senden"
        Me.bttBefehl_senden.Size = New System.Drawing.Size(118, 39)
        Me.bttBefehl_senden.TabIndex = 2
        Me.bttBefehl_senden.Text = "Befehl senden"
        Me.bttBefehl_senden.UseVisualStyleBackColor = True
        '
        'txtArgument
        '
        Me.txtArgument.Location = New System.Drawing.Point(136, 51)
        Me.txtArgument.Name = "txtArgument"
        Me.txtArgument.Size = New System.Drawing.Size(224, 20)
        Me.txtArgument.TabIndex = 4
        '
        'lbbCOMStatus
        '
        Me.lbbCOMStatus.AutoSize = True
        Me.lbbCOMStatus.Location = New System.Drawing.Point(9, 115)
        Me.lbbCOMStatus.Name = "lbbCOMStatus"
        Me.lbbCOMStatus.Size = New System.Drawing.Size(109, 13)
        Me.lbbCOMStatus.TabIndex = 5
        Me.lbbCOMStatus.Text = "Status: COM getrennt"
        '
        'rbbCOMStatus
        '
        Me.rbbCOMStatus.AutoSize = True
        Me.rbbCOMStatus.Enabled = False
        Me.rbbCOMStatus.Location = New System.Drawing.Point(124, 115)
        Me.rbbCOMStatus.Name = "rbbCOMStatus"
        Me.rbbCOMStatus.Size = New System.Drawing.Size(14, 13)
        Me.rbbCOMStatus.TabIndex = 6
        Me.rbbCOMStatus.TabStop = True
        Me.rbbCOMStatus.UseVisualStyleBackColor = True
        '
        'lbEmpfangeDaten
        '
        Me.lbEmpfangeDaten.FormattingEnabled = True
        Me.lbEmpfangeDaten.Location = New System.Drawing.Point(156, 77)
        Me.lbEmpfangeDaten.Name = "lbEmpfangeDaten"
        Me.lbEmpfangeDaten.Size = New System.Drawing.Size(328, 56)
        Me.lbEmpfangeDaten.TabIndex = 7
        '
        'lbbZyklusdaten
        '
        Me.lbbZyklusdaten.AutoSize = True
        Me.lbbZyklusdaten.Location = New System.Drawing.Point(12, 139)
        Me.lbbZyklusdaten.MaximumSize = New System.Drawing.Size(0, 100)
        Me.lbbZyklusdaten.Name = "lbbZyklusdaten"
        Me.lbbZyklusdaten.Size = New System.Drawing.Size(65, 13)
        Me.lbbZyklusdaten.TabIndex = 8
        Me.lbbZyklusdaten.Text = "Zyklusdaten"
        '
        'bttMotorenEINAUS
        '
        Me.bttMotorenEINAUS.BackColor = System.Drawing.Color.Red
        Me.bttMotorenEINAUS.Location = New System.Drawing.Point(396, 155)
        Me.bttMotorenEINAUS.Name = "bttMotorenEINAUS"
        Me.bttMotorenEINAUS.Size = New System.Drawing.Size(88, 39)
        Me.bttMotorenEINAUS.TabIndex = 9
        Me.bttMotorenEINAUS.Text = "Motoren AUS"
        Me.bttMotorenEINAUS.UseVisualStyleBackColor = False
        '
        'bttPID_Winkel_ändern
        '
        Me.bttPID_Winkel_ändern.Location = New System.Drawing.Point(11, 155)
        Me.bttPID_Winkel_ändern.Name = "bttPID_Winkel_ändern"
        Me.bttPID_Winkel_ändern.Size = New System.Drawing.Size(88, 39)
        Me.bttPID_Winkel_ändern.TabIndex = 10
        Me.bttPID_Winkel_ändern.Text = "PID-Winkel Nah ändern"
        Me.bttPID_Winkel_ändern.UseVisualStyleBackColor = True
        '
        'bttPID_Speed_ändern
        '
        Me.bttPID_Speed_ändern.Location = New System.Drawing.Point(199, 155)
        Me.bttPID_Speed_ändern.Name = "bttPID_Speed_ändern"
        Me.bttPID_Speed_ändern.Size = New System.Drawing.Size(88, 39)
        Me.bttPID_Speed_ändern.TabIndex = 11
        Me.bttPID_Speed_ändern.Text = "PID-Speed ändern"
        Me.bttPID_Speed_ändern.UseVisualStyleBackColor = True
        '
        'Akku_Timer
        '
        Me.Akku_Timer.Enabled = True
        Me.Akku_Timer.Interval = 10000
        '
        'LbbAkku
        '
        Me.LbbAkku.AutoSize = True
        Me.LbbAkku.Location = New System.Drawing.Point(393, 139)
        Me.LbbAkku.MaximumSize = New System.Drawing.Size(0, 100)
        Me.LbbAkku.Name = "LbbAkku"
        Me.LbbAkku.Size = New System.Drawing.Size(74, 13)
        Me.LbbAkku.TabIndex = 12
        Me.LbbAkku.Text = "Akku:XX.XX%"
        '
        'bttPIDFern
        '
        Me.bttPIDFern.Enabled = False
        Me.bttPIDFern.Location = New System.Drawing.Point(105, 155)
        Me.bttPIDFern.Name = "bttPIDFern"
        Me.bttPIDFern.Size = New System.Drawing.Size(88, 39)
        Me.bttPIDFern.TabIndex = 13
        Me.bttPIDFern.Text = "PID-Winkel Fern ändern"
        Me.bttPIDFern.UseVisualStyleBackColor = True
        '
        'bttAufnahme
        '
        Me.bttAufnahme.BackColor = System.Drawing.Color.Red
        Me.bttAufnahme.Location = New System.Drawing.Point(293, 155)
        Me.bttAufnahme.Name = "bttAufnahme"
        Me.bttAufnahme.Size = New System.Drawing.Size(88, 39)
        Me.bttAufnahme.TabIndex = 14
        Me.bttAufnahme.Text = "Aufnahme starten"
        Me.bttAufnahme.UseVisualStyleBackColor = False
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(509, 205)
        Me.Controls.Add(Me.bttAufnahme)
        Me.Controls.Add(Me.bttPIDFern)
        Me.Controls.Add(Me.LbbAkku)
        Me.Controls.Add(Me.bttPID_Speed_ändern)
        Me.Controls.Add(Me.bttPID_Winkel_ändern)
        Me.Controls.Add(Me.bttMotorenEINAUS)
        Me.Controls.Add(Me.lbbZyklusdaten)
        Me.Controls.Add(Me.lbEmpfangeDaten)
        Me.Controls.Add(Me.rbbCOMStatus)
        Me.Controls.Add(Me.lbbCOMStatus)
        Me.Controls.Add(Me.txtArgument)
        Me.Controls.Add(Me.cmbBefehl)
        Me.Controls.Add(Me.bttBefehl_senden)
        Me.Controls.Add(Me.cmbCOMPort)
        Me.Controls.Add(Me.bttCOMVerbinden)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.Name = "Form1"
        Me.Text = "Arduino COM Testprogramm"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents bttCOMVerbinden As System.Windows.Forms.Button
    Friend WithEvents cmbCOMPort As System.Windows.Forms.ComboBox
    Friend WithEvents cmbBefehl As System.Windows.Forms.ComboBox
    Friend WithEvents bttBefehl_senden As System.Windows.Forms.Button
    Friend WithEvents txtArgument As System.Windows.Forms.TextBox
    Friend WithEvents lbbCOMStatus As System.Windows.Forms.Label
    Friend WithEvents rbbCOMStatus As System.Windows.Forms.RadioButton
    Friend WithEvents lbEmpfangeDaten As System.Windows.Forms.ListBox
    Friend WithEvents lbbZyklusdaten As System.Windows.Forms.Label
    Friend WithEvents bttMotorenEINAUS As System.Windows.Forms.Button
    Friend WithEvents bttPID_Winkel_ändern As System.Windows.Forms.Button
    Friend WithEvents bttPID_Speed_ändern As System.Windows.Forms.Button
    Friend WithEvents Akku_Timer As System.Windows.Forms.Timer
    Friend WithEvents LbbAkku As System.Windows.Forms.Label
    Friend WithEvents bttPIDFern As Button
    Friend WithEvents bttAufnahme As System.Windows.Forms.Button
End Class
