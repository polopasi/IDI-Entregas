#include <QPushButton>
#include <string>

class MyQPushButton : public QPushButton {

    Q_OBJECT

    private:

    public:
        MyQPushButton (QWidget* parent);

    public slots:
        void cambiaColor (int color);
        
        void desactivar ();

        void activar ();

};