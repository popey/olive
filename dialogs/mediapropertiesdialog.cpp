#include "mediapropertiesdialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QTreeWidgetItem>

#include "io/media.h"
#include "panels/project.h"
#include "project/undo.h"

MediaPropertiesDialog::MediaPropertiesDialog(QWidget *parent, QTreeWidgetItem *i, Media *m) :
    QDialog(parent),
    item(i),
    media(m)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QGridLayout* grid = new QGridLayout();
	setLayout(grid);

    if (m->video_tracks.size() > 0) {
        interlacing_box = new QComboBox();
		interlacing_box->addItem("Auto (" + get_interlacing_name(media->video_tracks.at(0)->video_auto_interlacing) + ")");
        interlacing_box->addItem(get_interlacing_name(VIDEO_PROGRESSIVE));
        interlacing_box->addItem(get_interlacing_name(VIDEO_TOP_FIELD_FIRST));
        interlacing_box->addItem(get_interlacing_name(VIDEO_BOTTOM_FIELD_FIRST));

		interlacing_box->setCurrentIndex((media->video_tracks.at(0)->video_auto_interlacing == media->video_tracks.at(0)->video_interlacing) ? 0 : media->video_tracks.at(0)->video_interlacing + 1);

        grid->addWidget(new QLabel("Interlacing:"), 0, 0);
        grid->addWidget(interlacing_box, 0, 1);
    }

    name_box = new QLineEdit(m->name);
	grid->addWidget(new QLabel("Name:"), 1, 0);
    grid->addWidget(name_box, 1, 1);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	buttons->setCenterButtons(true);
	grid->addWidget(buttons, 2, 0, 1, 2);

	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void MediaPropertiesDialog::accept() {
	ComboAction* ca = new ComboAction();

	//set interlacing
	if (interlacing_box->currentIndex() > 0) {
		ca->append(new SetInt(&media->video_tracks.at(0)->video_interlacing, interlacing_box->currentIndex() - 1));
	} else {
		ca->append(new SetInt(&media->video_tracks.at(0)->video_interlacing, media->video_tracks.at(0)->video_auto_interlacing));
	}

	//set name
	MediaRename* mr = new MediaRename();
	mr->from = media->name;
	mr->item = item;
	mr->to = name_box->text();
	item->setText(0, name_box->text());

	ca->append(mr);
	ca->appendPost(new CloseAllClipsCommand());
	ca->appendPost(new UpdateFootageTooltip(item, media));

	undo_stack.push(ca);

    QDialog::accept();
}
