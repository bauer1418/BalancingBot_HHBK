Imports System
Imports CommandMessenger
Imports CommandMessenger.Transport.Serial

Public Class Form1
    Public COM_Port As IO.Ports.SerialPort = Nothing
    Dim TransportLayer As SerialTransport
    Dim Messenger As CmdMessenger

    Enum Befehle
        cmd_Statusmeldung           'Statusmeldungen z.B.: Setup beendet
        cmd_Akkustand               'Aktuelle Spannung des Primärakkus
        cmd_KalmanWinkel            'Aktueller Winkel aus dem Kalman Filter
        cmd_RAW_Werte               'RAW-Werte aus dem MPU ohne Bearbeitung
        cmd_Offset_Werte            'Offset-Werte für die XYZ Achsen des MPU6050
        cmd_MPU_Kalibrieren         'Automatik Kalibrierung durchführen

        cmd_PID_Winkel_MinMax       'Winkel Regler Min und Max Werte setzen
        cmd_PID_Winkel_Sollwert     'Winkel Regler Sollwert setzen
    End Enum

    Private Sub Form1_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
        Try
            If rbbCOMStatus.Checked = True Then
                Messenger.Disconnect()
                Messenger.Dispose()
                TransportLayer.Dispose()
                rbbCOMStatus.Checked = False
                lbbCOMStatus.Text = "Status: COM getrennt"
            End If
        Catch ex As Exception
            MsgBox("Fehler beim Schließen des COM Ports!" + vbNewLine + ex.ToString, vbCritical, "Fehler")
        End Try
    End Sub

    Private Sub Form1_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        For Each Port In My.Computer.Ports.SerialPortNames
            cmbCOMPort.Items.Add(Port)
        Next
        cmbCOMPort.SelectedItem = cmbCOMPort.Items(0)

        For Each Befehl In System.Enum.GetNames(GetType(Befehle))
            cmbBefehl.Items.Add(Befehl)
        Next
        cmbBefehl.SelectedItem = cmbBefehl.Items(0)
    End Sub

    Private Sub bttCOMVerbinden_Click(sender As Object, e As EventArgs) Handles bttCOMVerbinden.Click

        Try
            If rbbCOMStatus.Checked = False Then
                TransportLayer = New SerialTransport With {.CurrentSerialSettings = New SerialSettings With {.PortName = cmbCOMPort.SelectedItem, .BaudRate = 115200, .DtrEnable = False}}



                Messenger = New CmdMessenger(TransportLayer, BoardType.Bit16)
                lbbCOMStatus.Text = "Status: COM verbunden"
                rbbCOMStatus.Checked = True


                Messenger.ControlToInvokeOn = Me

                AttachCallbacks()
                AddHandler Messenger.NewLineReceived, AddressOf NewLineReceived
                AddHandler Messenger.NewLineSent, AddressOf NewLineSent

                Messenger.Connect()
            End If
        Catch ex As Exception
            rbbCOMStatus.Checked = False
            lbbCOMStatus.Text = "Status: COM getrennt"
            MsgBox("Fehler beim Öffnen des COM Ports!" + vbNewLine + ex.ToString, vbCritical, "Fehler")
        End Try
    End Sub

    Private Sub bttBefehl_senden_Click(sender As Object, e As EventArgs) Handles bttBefehl_senden.Click
        If rbbCOMStatus.Checked = True Then
            Dim Command = New SendCommand(cmbBefehl.SelectedIndex, False)

            Messenger.SendCommand(Command)
        Else
            MsgBox("Arduino nicht verbunden!", vbCritical, "Keine Verbindung")
        End If
    End Sub
    Private Sub AttachCallbacks()
        Messenger.Attach(AddressOf OnUnknownCommand)
        Messenger.Attach(Befehle.cmd_Statusmeldung, AddressOf Statusmeldung)
        Messenger.Attach(Befehle.cmd_Akkustand, AddressOf Akkustatus)
    End Sub

    Private Sub NewLineReceived(ByVal sender As Object, ByVal e As CommandEventArgs)

    End Sub
    Private Sub NewLineSent(ByVal sender As Object, ByVal e As CommandEventArgs)

    End Sub


    '-------------------CALLBACKS----------------------
    Private Sub OnUnknownCommand(ByVal arguments As ReceivedCommand)
        MsgBox("Unbekannter Befehl empfangen", vbCritical, "Fehler")
    End Sub
    Private Sub Statusmeldung(ByVal arguments As ReceivedCommand)
        'MsgBox("Empfangen:" + arguments.ReadBinStringArg, vbInformation)
        lbEmpfangeDaten.Items.Add(arguments.ReadBinStringArg)
        lbEmpfangeDaten.SelectedIndex = lbEmpfangeDaten.Items.Count - 1
    End Sub
    Private Sub Akkustatus(ByVal arguments As ReceivedCommand)
        MsgBox(arguments.CmdId)
        lbEmpfangeDaten.Items.Add("Akkuspannung: " + arguments.ReadFloatArg().ToString("0.00V"))
        lbEmpfangeDaten.SelectedIndex = lbEmpfangeDaten.Items.Count - 1
    End Sub

End Class
