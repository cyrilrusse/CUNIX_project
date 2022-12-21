#ifndef WINDOWGERANT_H
#define WINDOWGERANT_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class WindowGerant; }
QT_END_NAMESPACE

class WindowGerant : public QMainWindow
{
    Q_OBJECT

public:
    WindowGerant(QWidget *parent = nullptr);
    ~WindowGerant();

    void ajouteArticleTablePanier(int id, const char *article, float prix, int quantite);
    void videTableStock();
    int getIndiceArticleSelectionne();

    float getPrix();
    int getStock();
    const char *getPublicite();

    void closeEvent(QCloseEvent *event);

private slots:
    void on_tableWidgetStock_cellClicked(int row, int column);
    void on_pushButtonPublicite_clicked();
    void on_pushButtonModifier_clicked();

private:
    Ui::WindowGerant *ui;

    char publicite[80];
};
#endif // WINDOWGERANT_H
