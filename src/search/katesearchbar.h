/*  This file is part of the KDE libraries and the Kate part.
 *
 *  Copyright (C) 2009-2010 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
 *  Copyright (C) 2007 Sebastian Pipping <webmaster@hartwork.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KATE_SEARCH_BAR_H
#define KATE_SEARCH_BAR_H 1

#include "kateviewhelpers.h"
#include <ktexteditor_export.h>

#include <ktexteditor/attribute.h>
#include <ktexteditor/document.h>

namespace KTextEditor { class ViewPrivate; }
class KateViewConfig;
class QVBoxLayout;
class QComboBox;

namespace Ui
{
class IncrementalSearchBar;
class PowerSearchBar;
}

namespace KTextEditor
{
class MovingRange;
class Message;
}

class KTEXTEDITOR_EXPORT KateSearchBar : public KateViewBarWidget
{
    Q_OBJECT

    friend class SearchBarTest;

public:
    enum SearchMode {
        // NOTE: Concrete values are important here
        // to work with the combobox index!
        MODE_PLAIN_TEXT = 0,
        MODE_WHOLE_WORDS = 1,
        MODE_ESCAPE_SEQUENCES = 2,
        MODE_REGEX = 3
    };

    enum MatchResult {
        MatchFound,
        MatchWrappedForward,
        MatchWrappedBackward,
        MatchMismatch,
        MatchNothing,
        MatchNeutral
    };

    enum SearchDirection {
        SearchForward,
        SearchBackward
    };

public:
    explicit KateSearchBar(bool initAsPower, KTextEditor::ViewPrivate *view, KateViewConfig *config);
    ~KateSearchBar();

    void closed() Q_DECL_OVERRIDE;

    bool isPower() const;

    QString searchPattern() const;
    QString replacementPattern() const;

    bool selectionOnly() const;
    bool matchCase() const;

    // Only used by KTextEditor::ViewPrivate
    static void nextMatchForSelection(KTextEditor::ViewPrivate *view, SearchDirection searchDirection);

public Q_SLOTS:
    /**
     * Set the current search pattern.
     * @param searchPattern the search pattern
     */
    void setSearchPattern(const QString &searchPattern);

    /**
     * Set the current replacement pattern.
     * @param replacementPattern the replacement pattern
     */
    void setReplacementPattern(const QString &replacementPattern);

    void setSearchMode(SearchMode mode);
    void setSelectionOnly(bool selectionOnly);
    void setMatchCase(bool matchCase);

    // Called for <F3> and <Shift>+<F3>
    void findNext();
    void findPrevious();
    void findAll();

    void replaceNext();
    void replaceAll();

    // Also used by KTextEditor::ViewPrivate
    void enterPowerMode();
    void enterIncrementalMode();

    bool clearHighlights();
    void updateHighlightColors();

    // read write status of document changed
    void slotReadWriteChanged();

protected:
    // Overridden
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onIncPatternChanged(const QString &pattern);
    void onMatchCaseToggled(bool matchCase);

    void onReturnPressed();
    void updateSelectionOnly();
    void updateIncInitCursor();

    void onPowerPatternChanged(const QString &pattern);

    void onPowerModeChanged(int index);
    void onPowerPatternContextMenuRequest();
    void onPowerPatternContextMenuRequest(const QPoint &);
    void onPowerReplacmentContextMenuRequest();
    void onPowerReplacmentContextMenuRequest(const QPoint &);

private:
    // Helpers
    bool find(SearchDirection searchDirection = SearchForward, const QString *replacement = 0);
    int findAll(KTextEditor::Range inputRange, const QString *replacement);

    bool isPatternValid() const;

    KTextEditor::SearchOptions searchOptions(SearchDirection searchDirection = SearchForward) const;

    void highlightMatch(const KTextEditor::Range &range);
    void highlightReplacement(const KTextEditor::Range &range);
    void indicateMatch(MatchResult matchResult);
    static void selectRange(KTextEditor::ViewPrivate *view, const KTextEditor::Range &range);
    void selectRange2(const KTextEditor::Range &range);

    QVector<QString> getCapturePatterns(const QString &pattern) const;
    void showExtendedContextMenu(bool forPattern, const QPoint &pos);

    void givePatternFeedback();
    void addCurrentTextToHistory(QComboBox *combo);
    void backupConfig(bool ofPower);
    void sendConfig();
    void fixForSingleLine(KTextEditor::Range &range, SearchDirection searchDirection);

    void showInfoMessage(const QString &text);

private:
    KTextEditor::ViewPrivate *const m_view;
    KateViewConfig *const m_config;
    QList<KTextEditor::MovingRange *> m_hlRanges;
    QPointer<KTextEditor::Message> m_infoMessage;
    QPointer<KTextEditor::Message> m_wrappedTopMessage;
    QPointer<KTextEditor::Message> m_wrappedBottomMessage;

    // Shared by both dialogs
    QVBoxLayout *const m_layout;
    QWidget *m_widget;

    // Incremental search related
    Ui::IncrementalSearchBar *m_incUi;
    KTextEditor::Cursor m_incInitCursor;

    // Power search related
    Ui::PowerSearchBar *m_powerUi;

    // attribute to highlight matches with
    KTextEditor::Attribute::Ptr highlightMatchAttribute;
    KTextEditor::Attribute::Ptr highlightReplacementAttribute;

    // Status backup
    bool m_incHighlightAll : 1;
    bool m_incFromCursor : 1;
    bool m_incMatchCase : 1;
    bool m_powerMatchCase : 1;
    bool m_powerFromCursor : 1;
    bool m_powerHighlightAll : 1;
    unsigned int m_powerMode : 2;
};

#endif // KATE_SEARCH_BAR_H

