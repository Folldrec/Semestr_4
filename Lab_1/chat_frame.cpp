#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/textdlg.h>
#include "patterns/chat_facade.hpp"

enum { ID_BTN_SEND = 1001, ID_BTN_CONNECT = 1002,
       ID_BTN_UNDO = 1003, ID_CHK_TIMESTAMP = 1004,
       ID_CHK_ENCRYPT = 1005 };

/**
 * @brief Status bar observer — reacts to network events.
 * Pattern: Observer (Behavioral) — concrete observer
 */
class StatusBarObserver : public IChatObserver {
    wxStatusBar* m_bar;
public:
    explicit StatusBarObserver(wxStatusBar* bar) : m_bar(bar) {}

    void onEvent(const ChatEventData& e) override {
        wxTheApp->CallAfter([this, e]() {
            switch (e.type) {
                case ChatEvent::PeerConnected:
                    m_bar->SetStatusText("Connected to " + wxString::FromUTF8(e.payload));
                    break;
                case ChatEvent::MessageSent:
                    m_bar->SetStatusText("Message sent");
                    break;
                case ChatEvent::MessageReceived:
                    m_bar->SetStatusText("New message received");
                    break;
                case ChatEvent::ServerStarted:
                    m_bar->SetStatusText("Server listening on port " + wxString::FromUTF8(e.payload));
                    break;
                default: break;
            }
        });
    }
};

/**
 * @brief Main window — only handles UI.
 * All logic delegated to ChatFacade.
 */
class ChatFrame : public wxFrame {
public:
    ChatFrame(const wxString& nickname)
        : wxFrame(nullptr, wxID_ANY, "C++ P2P Chat",
                  wxDefaultPosition, wxSize(560, 460)),
          m_nickname(nickname)
    {
        SetTitle(wxString::Format("P2P Chat — %s", m_nickname));
        CreateStatusBar();

        m_facade = std::make_unique<ChatFacade>();
        m_facade->setUICallbacks(
            [this](const std::string& msg) {
                wxTheApp->CallAfter([this, msg]() {
                    txtChatHistory->AppendText(wxString::FromUTF8(msg) + "\n");
                });
            },
            [this]() {
                wxTheApp->CallAfter([this]() {
                    long last = txtChatHistory->GetNumberOfLines() - 2;
                    if (last >= 0) {
                        long pos = txtChatHistory->XYToPosition(0, last);
                        txtChatHistory->Remove(pos, txtChatHistory->GetLastPosition());
                    }
                });
            }
        );

        m_statusObserver = std::make_unique<StatusBarObserver>(GetStatusBar());
        m_facade->subscribeObserver(m_statusObserver.get());

        wxPanel* panel = new wxPanel(this);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer* connSizer = new wxBoxSizer(wxHORIZONTAL);
        txtMyPort      = new wxTextCtrl(panel, wxID_ANY, "8080", wxDefaultPosition, wxSize(60,-1));
        btnStartServer = new wxButton(panel, wxID_ANY, "Start Server");
        txtTargetIP    = new wxTextCtrl(panel, wxID_ANY, "127.0.0.1");
        txtTargetPort  = new wxTextCtrl(panel, wxID_ANY, "8081", wxDefaultPosition, wxSize(60,-1));
        btnConnect     = new wxButton(panel, ID_BTN_CONNECT, "Connect");

        connSizer->Add(new wxStaticText(panel, wxID_ANY, "Port:"), 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
        connSizer->Add(txtMyPort, 0, wxALL, 4);
        connSizer->Add(btnStartServer, 0, wxALL, 4);
        connSizer->Add(new wxStaticLine(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), 0, wxEXPAND|wxALL, 4);
        connSizer->Add(new wxStaticText(panel, wxID_ANY, "Peer IP:"), 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
        connSizer->Add(txtTargetIP, 1, wxALL, 4);
        connSizer->Add(txtTargetPort, 0, wxALL, 4);
        connSizer->Add(btnConnect, 0, wxALL, 4);

        wxBoxSizer* optSizer = new wxBoxSizer(wxHORIZONTAL);
        chkTimestamp = new wxCheckBox(panel, ID_CHK_TIMESTAMP, "Timestamps");
        chkEncrypt   = new wxCheckBox(panel, ID_CHK_ENCRYPT,   "Encrypt marker");
        chkTimestamp->SetValue(true);
        optSizer->Add(chkTimestamp, 0, wxALL, 4);
        optSizer->Add(chkEncrypt,   0, wxALL, 4);

        txtChatHistory = new wxTextCtrl(panel, wxID_ANY, "",
                                        wxDefaultPosition, wxDefaultSize,
                                        wxTE_MULTILINE | wxTE_READONLY);

        wxBoxSizer* msgSizer = new wxBoxSizer(wxHORIZONTAL);
        txtMessage = new wxTextCtrl(panel, wxID_ANY,
                                    wxString::FromUTF8("Привіт!"),
                                    wxDefaultPosition, wxDefaultSize,
                                    wxTE_PROCESS_ENTER);
        btnSend = new wxButton(panel, ID_BTN_SEND, "Send");
        btnUndo = new wxButton(panel, ID_BTN_UNDO, "Undo");

        msgSizer->Add(txtMessage, 1, wxALL, 4);
        msgSizer->Add(btnSend, 0, wxALL, 4);
        msgSizer->Add(btnUndo, 0, wxALL, 4);

        mainSizer->Add(connSizer,      0, wxEXPAND | wxALL, 4);
        mainSizer->Add(optSizer,       0, wxEXPAND | wxALL, 4);
        mainSizer->Add(txtChatHistory, 1, wxEXPAND | wxALL, 4);
        mainSizer->Add(msgSizer,       0, wxEXPAND | wxALL, 4);
        panel->SetSizer(mainSizer);

        btnStartServer->Bind(wxEVT_BUTTON,    &ChatFrame::OnStartServer, this);
        btnConnect->Bind(wxEVT_BUTTON,        &ChatFrame::OnConnect,     this);
        btnSend->Bind(wxEVT_BUTTON,           &ChatFrame::OnSend,        this);
        btnUndo->Bind(wxEVT_BUTTON,           &ChatFrame::OnUndo,        this);
        txtMessage->Bind(wxEVT_TEXT_ENTER,    &ChatFrame::OnSend,        this);
        chkTimestamp->Bind(wxEVT_CHECKBOX,    &ChatFrame::OnToggleTimestamp, this);
        chkEncrypt->Bind(wxEVT_CHECKBOX,      &ChatFrame::OnToggleEncrypt,   this);
    }

private:
    void OnStartServer(wxCommandEvent&) {
        long port;
        if (txtMyPort->GetValue().ToLong(&port)) {
            m_facade->startServer(static_cast<unsigned short>(port));
            btnStartServer->Disable();
        }
    }

    void OnConnect(wxCommandEvent&) {
        m_facade->connectToPeer(
            std::string(txtTargetIP->GetValue().ToUTF8()),
            std::string(txtTargetPort->GetValue().ToUTF8())
        );
    }

    void OnSend(wxCommandEvent&) {
        wxString wxMsg = txtMessage->GetValue();
        if (wxMsg.empty()) return;
        m_facade->sendMessage(
            std::string(m_nickname.ToUTF8()),
            std::string(wxMsg.ToUTF8())
        );
        txtMessage->Clear();
    }

    void OnUndo(wxCommandEvent&) {
        if (m_facade->canUndo())
            m_facade->undoLastMessage();
    }

    void OnToggleTimestamp(wxCommandEvent&) {
        m_facade->setTimestamps(chkTimestamp->IsChecked());
    }

    void OnToggleEncrypt(wxCommandEvent&) {
        m_facade->setEncryption(chkEncrypt->IsChecked());
    }

    wxTextCtrl* txtMyPort;
    wxButton*   btnStartServer;
    wxTextCtrl* txtTargetIP;
    wxTextCtrl* txtTargetPort;
    wxButton*   btnConnect;
    wxTextCtrl* txtChatHistory;
    wxTextCtrl* txtMessage;
    wxButton*   btnSend;
    wxButton*   btnUndo;
    wxCheckBox* chkTimestamp;
    wxCheckBox* chkEncrypt;

    wxString                          m_nickname;
    std::unique_ptr<ChatFacade>       m_facade;
    std::unique_ptr<StatusBarObserver> m_statusObserver;
};

class ChatApp : public wxApp {
public:
    virtual bool OnInit() override {
        wxInitAllImageHandlers();
        wxString nick = wxString::FromUTF8("Гість");
        wxTextEntryDialog dlg(nullptr,
            wxString::FromUTF8("Введіть нікнейм:"),
            wxString::FromUTF8("Вхід"), nick, wxOK|wxCANCEL|wxCENTRE);
        if (dlg.ShowModal() == wxID_OK) {
            if (!dlg.GetValue().IsEmpty()) nick = dlg.GetValue();
            (new ChatFrame(nick))->Show(true);
            return true;
        }
        return false;
    }
};

wxIMPLEMENT_APP(ChatApp);