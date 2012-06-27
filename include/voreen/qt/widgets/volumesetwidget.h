/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2009 Visualization and Computer Graphics Group, *
 * Department of Computer Science, University of Muenster, Germany.   *
 * <http://viscg.uni-muenster.de>                                     *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#ifndef VRN_VOLUMESETWIDGET_H
#define VRN_VOLUMESETWIDGET_H

#include <string>
#include <vector>
#include <typeinfo>

#include <QDialog>
#include <QTreeWidgetItem>

#include "voreen/core/volume/observer.h"

#ifdef VRN_WITH_DCMTK
#include "voreen/core/io/dicomvolumereader.h"
#include "voreen/qt/dicomdialog.h"
#endif

// forward declarations for speed
class QComboBox;
class QGridLayout;
class QGroupBox;
class QPushButton;

namespace voreen {

class IOProgressDialog;
class VolumeHandle;
class VolumeSerializerPopulator;
class VolumeSeries;
class VolumeSet;
class VolumeSetContainer;

/**
 * Class for managing currently loaded VolumeSet files in Voreen applications.
 *
 * @author Dirk Feldmann, 2008
 */
class VolumeSetWidget : public QDialog, public Observer {
    Q_OBJECT
public:
    /**
     * Allows to omit certain levels in the hierarchy of the VolumeSet concept. E.g. if
     * an application only deals with volume data of the type "CT", you need not to create
     * the hierarchy level for VolumeSeries, as there is only one. The the timesteps from
     * the VolumeHandles would be displayed directly below the VolumeSet and would be added
     * there.
     */
    enum AvailableLevels {
        LEVELS_NONE = 0,
        LEVEL_VOLUMESETS = 1,
        LEVEL_VOLUMESERIES = 2,
        LEVEL_VOLUMEHANDLES = 4,
        LEVEL_ALL = 7
    };

    /**
     * Ctor.
     * @param   volumeSetContainer  The VolumeSetContainer which shall be managed.
     * @param   parent  The parent widget of this widget. If parent is NULL, the widget
     *  is displayed in its own window.
     * @param   leves   levels of the hierarchy which shall be displayed and managed.
     * @param   flags   QWindow flags for the window appearance
     */
    VolumeSetWidget(VolumeSetContainer* const volumeSetContainer, QWidget* parent = 0,
        int levels = LEVEL_ALL, bool allowClose = false, Qt::WFlags flags = Qt::Tool);

    /**
     * Dtor. Does not delete the VolumeSetContainer.
     */
    ~VolumeSetWidget();

    /**
     * Sets the VolumeSetContainer.
     */
    void setVolumeSetContainer(VolumeSetContainer* vsc);

    /**
     * Sets whether multiple file selection is enabled or disabled.
     */
    void setAllowAddingMultipleFiles(const bool allow);

    /**
     * Indicates whether multiple file selection is enabled or disabled.
     */
    bool getAllowAddingMultipleFiles() const;

    /**
     * Sets the current directory which is used for file selection.
     */
    void setCurrentDirectory(const std::string& dir);

    /**
     * Returns the current directory which is used for file selection.
     */
    const std::string& getCurrentDirectory() const;

    /**
     * Sets the filter for allowed file extension on file selection.
     */
    void setFileExtensionFilter(const std::string& fileExtFilter);

    /**
     * Returns the filter for allowed file extension on file selection.
     */
    const std::string& getFileExtensionFilter() const;

    /**
     * Adds the content of the files from the given file names to the VolumeSetContainer
     * as new VolumeSet objects. A VolumeSet is created for each file.
     */
    void addVolumeSets(const std::vector<std::string>& filenames);

    /**
     * Adds the content of the files from the given file names to the given VolumeSet as new
     * VolumeSeries objects. A VolumeSeries is created for each file.
     */
    void addVolumeSeries(const std::vector<std::string>& filenames,
                         VolumeSet* parentVolumeSet);

    /**
     * Adds the content of the files from the given file names to the given VolumeSeries as new
     * VolumeHandle objects. A VolumeHandle is created for each file.
     */
    void addVolumeHandles(const std::vector<std::string>& filenames,
                          VolumeSeries* parentVolumeSeries);

    /**
     * Directly add the content of the file from the given file name to the VolumeSetContainer
     * as new VolumeSet and returns it. Use this method for initial loading data sets from your
     * application.
     */
    VolumeSet* loadVolumeSet(const std::string& filename, const bool addToContainer = true);

    void loadDicomDir(const std::string& file);
    void loadDicomFiles(const std::string& dir);

    /** Shows a dialog for opening files and returns the selected file names.
     * Use <code>setAllowMultipleFileSelection()</code> and <code>setFileExtensionFilter()</code>
     * to determine the capability of multiple file selection and the supported file extensions.
     */
    std::vector<std::string> openFileDialog();

    /**
     * Clean up - delete all volumesets
     */
    void clear();

    /**
     * Show progress bar while loading?
     */
    void setUseProgress(bool useProgress) { useProgress_ = useProgress; }

    /**
     * Updates content and causes a repaint, called via the VolumeSet observer.
     */    
    virtual void notify(const Observable* const source = 0);
                                                           
public slots:
    /**
     * Clears the QTreeWidget and updates its content based on the current
     * content of the VolumeSetContainer. This method is called by
     * <code>notify()</code> which has been inherited from class Observer.
     */
    void updateContent();

    /**
     * The action performed when clicking the button for loading files.
     */
    //FIXME: say what they do, not when they are called. joerg
    void buttonAddClicked();

    void buttonAddDICOMClicked();
    void buttonAddDICOMDirClicked();

    /**
     * The action performed when clicking the button for removing data sets.
     */
    void buttonRemoveClicked();

    void comboModalitySelectionChanged(const QString& text);

    void dicomDirDialogFinished();

    /**
     * The action performed when altering the selection of the QTreeWidget.
     */
    void treeItemSelectionChanged();

    void treeItemChanged(QTreeWidgetItem* item, int column);

    /**
     * Prevent strange behavior if ESC is hit.
     */
    virtual void reject() { }

signals:
    void volumeSetChanged();
    
protected:
    /**
     * Abstract base class for QTreeWidgetItems. This only necessary
     * in order to have a common method <code>getType()</code> which
     * classifies the data contained by the different
     * TreeWidgetItems / nodes.
     */
    class AbstractVolumeSetTreeItem : public QTreeWidgetItem {
    public:
        virtual ~AbstractVolumeSetTreeItem() {};
        virtual const std::type_info& getType() = 0;
    };

    /**
     * Class for the QTreeWidgetItems containing additionally a
     * pointer to the objects they are associated with.
     * The QTreeWidget may contain nodes which can be either
     * - the root (no data associated, void pointer)
     * - VolumeSet items (VolumeSet pointers associated)
     * - VolumeSeries items
     * - VolumeHandle items
     */
    template<class T>
    class VolumeSetTreeItem : public AbstractVolumeSetTreeItem {
    public:
        VolumeSetTreeItem(const std::string& lable, T itemData)
            : AbstractVolumeSetTreeItem(),
              itemData_(itemData),
              type_(typeid(itemData))
            {
                QTreeWidgetItem::setText(0, tr(lable.c_str()));
            }

        virtual ~VolumeSetTreeItem() { }

        T getItemData() { return itemData_; }
        const std::type_info& getType() { return type_; }

    protected:
        T itemData_;
        const std::type_info& type_;
    };

    // Special instances of the template items described
    // above for convenience
    //
    typedef VolumeSetTreeItem<void*> VolumeRootItem;
    typedef VolumeSetTreeItem<VolumeSet*> VolumeSetItem;
    typedef VolumeSetTreeItem<VolumeSeries*> VolumeSeriesItem;
    typedef VolumeSetTreeItem<VolumeHandle*> VolumeHandleItem;

    /**
     * QTreeWidget class containing one column and no fancy stuff.
     */
    class VolumeSetTreeWidget : public QTreeWidget {
    public:
        VolumeSetTreeWidget(QWidget* parent = 0);

    protected:
        virtual void dragEnterEvent(QDragEnterEvent* event);
        virtual void dragMoveEvent(QDragMoveEvent* event);
        virtual bool dropMimeData(QTreeWidgetItem* parent, int index,
                                  const QMimeData* data, Qt::DropAction action);
        virtual void mousePressEvent(QMouseEvent* event);

    private:
        void moveItem(VolumeSeriesItem* child, VolumeSetItem* newParent);
        void moveItem(VolumeHandleItem* child, VolumeSeriesItem* newParent);
    };

    VolumeSerializerPopulator* volumeSerializerPopulator_;
    VolumeSetContainer* volumeSetContainer_;

    // appearance controlling
    //
    int availableLevelsMask_;
    std::string fileExtFilter_;
    std::string currentDir_;
    bool allowAddingMultipleFiles_;

    // Qt controls
    //
    QComboBox* comboModality_;
    QGridLayout* layout_;
    QGroupBox* groupAdd_;
    QGroupBox* groupRemove_;
    QGroupBox* groupModality_;
    QPushButton* btnLoad_;
    QPushButton* btnLoadDICOM_;
    QPushButton* btnLoadDICOMDir_;
    QPushButton* btnUnload_;
    VolumeSetTreeWidget* treeVolumeSets_;

    // DICOM dialog
    //
#ifdef VRN_WITH_DCMTK
    DicomDirDialog* dicomDirDialog_;
#endif

private:
    IOProgressDialog* progress_;

    typedef std::map<std::string, int> ModalityIndexMap;
    ModalityIndexMap modalityIndices_;

    bool allowClose_;
    bool useProgress_;

    void appendVolumeSets(VolumeRootItem* node);
    void appendVolumeSeries(QTreeWidgetItem* node, VolumeSet* const volumeSet);
    void appendVolumeHandles(QTreeWidgetItem* node, VolumeSeries* const volumeSeries);
    void setModalityComboIndex(const std::string& modalityName);
};

} // namespace

#endif // VRN_VOLUMESETWIDGET_H