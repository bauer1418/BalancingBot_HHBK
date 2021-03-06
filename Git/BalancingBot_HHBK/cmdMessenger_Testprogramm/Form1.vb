﻿Imports System
Imports CommandMessenger
Imports CommandMessenger.Transport.Serial

Public Class Form1
    Public COM_Port As IO.Ports.SerialPort = Nothing
    Dim TransportLayer As SerialTransport
    Dim Messenger As CmdMessenger

    Enum Befehle

        cmd_Statusmeldung           'Statusmeldungen z.B.: Setup beendet
        cmd_Fehlermeldung           'Fehlermeldungen ausgeben
        cmd_Akkustand_Prozent       'Aktuelle Spannung des Akkupacks in %
        cmd_KalmanWinkel            'Aktueller Winkel aus dem Kalman Filter
        'cmd_RAW_Werte				RAW-Werte aus dem MPU ohne Bearbeitung
        cmd_Offset_Werte            'Offset-Werte für die XYZ Achsen des MPU6050
        cmd_MPU_Kalibrieren     'Automatik Kalibrierung durchführen
        cmd_PID_Winkel_MinMax       'Winkel Regler Min und Max Werte setzen
        cmd_PID_Winkel_Sollwert 'Winkel Regler Sollwert setzen
        cmd_Umkipperkennung_quitt   'Umkipperkennung zurücksetzen
        cmd_MPU_Temperatur          'MPU Temperatur senden
        cmd_MotorenEINAUS           'Motoren Status lesen
        cmd_Anzeige_Text            'Anzuzeigender Text aus dem Arduino
        cmd_Zyklusdaten             'Zyklusdaten aus dem Arduino
        cmd_PID_Werte               'PID Werte verstellen oder auslesen 1=Werte setzen Winkelregler 2=Werte setzen Geschwindigkeitsregler 3= Werte lesen Winkelregler 4=Werte lesen Geschwindigkeitsregler
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
        Try
            cmbCOMPort.SelectedItem = cmbCOMPort.Items(0)
        Catch
            MsgBox("Keine COM-Ports gefunden", MsgBoxStyle.Critical, "Fehler")
        End Try
        For Each Befehl In System.Enum.GetNames(GetType(Befehle))
            cmbBefehl.Items.Add(Befehl)
        Next
        cmbBefehl.SelectedItem = cmbBefehl.Items(0)
    End Sub

    Private Sub bttCOMVerbinden_Click(sender As Object, e As EventArgs) Handles bttCOMVerbinden.Click

        Try
            If rbbCOMStatus.Checked = False Then
                TransportLayer = New SerialTransport With {.CurrentSerialSettings = New SerialSettings With {.PortName = cmbCOMPort.SelectedItem, .BaudRate = 57600, .DtrEnable = False}}



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
            Dim Command
            If txtArgument.Text <> "" Then
                Command = New SendCommand(cmbBefehl.SelectedIndex, txtArgument.Text)
            Else
                Command = New SendCommand(cmbBefehl.SelectedIndex)
            End If
            Messenger.SendCommand(Command)
        Else
            MsgBox("Arduino nicht verbunden!", vbCritical, "Keine Verbindung")
        End If
    End Sub
    Private Sub AttachCallbacks()
        Messenger.Attach(AddressOf OnUnknownCommand)
        Messenger.Attach(Befehle.cmd_Statusmeldung, AddressOf Statusmeldung)
        Messenger.Attach(Befehle.cmd_Fehlermeldung, AddressOf Neue_Daten_Float)
        Messenger.Attach(Befehle.cmd_Akkustand_Prozent, AddressOf Akkustatus)
        Messenger.Attach(Befehle.cmd_KalmanWinkel, AddressOf Neue_Daten_Float)
        Messenger.Attach(Befehle.cmd_MPU_Temperatur, AddressOf Neue_Daten_Float)
        Messenger.Attach(Befehle.cmd_MotorenEINAUS, AddressOf Neue_Daten_Float)
        Messenger.Attach(Befehle.cmd_Anzeige_Text, AddressOf Neue_Daten_String)
        Messenger.Attach(Befehle.cmd_MPU_Kalibrieren, AddressOf Neue_Daten_Float)
        Messenger.Attach(Befehle.cmd_Offset_Werte, AddressOf Neue_Daten_Float)
        Messenger.Attach(Befehle.cmd_Zyklusdaten, AddressOf Zyklusdaten_auswerten)
        Messenger.Attach(Befehle.cmd_PID_Winkel_MinMax, AddressOf PID_Winkel_MinMax)
        Messenger.Attach(Befehle.cmd_PID_Winkel_Sollwert, AddressOf PID_Winkel_Sollwert)
        Messenger.Attach(Befehle.cmd_PID_Werte, AddressOf PID_Werte)
    End Sub

    Private Sub NewLineReceived(ByVal sender As Object, ByVal e As CommandEventArgs)
        'lbbCOMStatus_Byte.Text = COM_Port.BytesToRead.ToString + "Byte von " + COM_Port.ReadBufferSize.ToString + "Byte"
        'lbbCOMStatus_Prozent.Text = "Bufferauslastung: " + COM_Port.BytesToRead / COM_Port.ReadBufferSize * 100 + "%"
        'lbbCOMStatus_Prozent.Text = TransportLayer.
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
        lbEmpfangeDaten.Items.Add("Akkustand: " + arguments.ReadFloatArg().ToString)
        lbEmpfangeDaten.SelectedIndex = lbEmpfangeDaten.Items.Count - 1
    End Sub
    Private Sub Neue_Daten_String(ByVal arguments As ReceivedCommand)
        While arguments.Available = True
            lbEmpfangeDaten.Items.Add(arguments.ReadStringArg().ToString)
            lbEmpfangeDaten.SelectedIndex = lbEmpfangeDaten.Items.Count - 1
        End While
    End Sub
    Private Sub Neue_Daten_Float(ByVal arguments As ReceivedCommand)
        While arguments.Available = True
            lbEmpfangeDaten.Items.Add(arguments.ReadFloatArg().ToString)
            lbEmpfangeDaten.SelectedIndex = lbEmpfangeDaten.Items.Count - 1
        End While
    End Sub
    Private Sub Zyklusdaten_auswerten(ByVal arguments As ReceivedCommand)
        Dim Winkel, PIDOut As Double
        Dim Zykluszeit As Integer
        Dim Motorenstatus As Boolean
        Dim Systemstatus As Integer
        Winkel = arguments.ReadFloatArg()
        PIDOut = arguments.ReadFloatArg()
        Motorenstatus = arguments.ReadBoolArg()
        Zykluszeit = arguments.ReadUInt32Arg()
        Systemstatus = arguments.ReadInt32Arg()

        lbbZyklusdaten.Text = ("Winkel:" + Winkel.ToString("0.00°") + " PIDOUT:" + PIDOut.ToString("00.00") + " Motoren:" + Motorenstatus.ToString + " Zykluszeit:" + Zykluszeit.ToString + "µs")

    End Sub
    Private Sub PID_Winkel_MinMax(ByVal arguments As ReceivedCommand)
        'MsgBox(arguments.CmdId)
        MsgBox("Neue PID Winkel Min,Max Werte:" + arguments.ReadDoubleArg.ToString + "," + arguments.ReadDoubleArg.ToString)
    End Sub
    Private Sub PID_Winkel_Sollwert(ByVal arguments As ReceivedCommand)
        'MsgBox(arguments.CmdId)
        MsgBox("PID-Winkel-Sollwert:" + arguments.ReadDoubleArg.ToString)
    End Sub
    Private Sub PID_Werte(ByVal arguments As ReceivedCommand)
        'MsgBox(arguments.CmdId)
        Dim Einstellung As Integer
        Dim P, I, D As Double
        Einstellung = arguments.ReadInt16Arg()
        P = arguments.ReadDoubleArg()
        I = arguments.ReadDoubleArg()
        D = arguments.ReadDoubleArg()
        If Einstellung = 1 Or Einstellung = 3 Then
            MsgBox("PID-Regler Winkel Einstellungen:" + vbNewLine + "P:" + P.ToString + vbNewLine + "I:" + I.ToString + vbNewLine + "D:" + D.ToString)
        ElseIf Einstellung = 2 Or Einstellung = 4 Then
            MsgBox("PID-Regler Geschwindigkeit Einstellungen:" + vbNewLine + "P:" + P.ToString + vbNewLine + "I:" + I.ToString + vbNewLine + "D:" + D.ToString)
        Else
            MsgBox("Unbekannte Reglernummer empfangen " + Einstellung.ToString)
        End If

    End Sub
End Class

