#ifndef WITHANOTHERPLAYER_H
#define WITHANOTHERPLAYER_H

#include <QDialog>
#include <QPixmap>

namespace Ui {
class WithAnotherPlayer;
}

class WithAnotherPlayer : public QDialog
{
    Q_OBJECT

public:
    enum WithAnotherPlayerType
    {
        WithAnotherPlayerType_Trade,
        WithAnotherPlayerType_Battle
    };
    explicit WithAnotherPlayer(QWidget *parent, const WithAnotherPlayerType &type, const QPixmap &skin, const QString &pseudo);
    ~WithAnotherPlayer();
    bool actionIsAccepted();
private slots:
    void on_yes_clicked();
    void on_no_clicked();
private:
    Ui::WithAnotherPlayer *ui;
    bool actionAccepted;
};

#endif // WITHANOTHERPLAYER_H