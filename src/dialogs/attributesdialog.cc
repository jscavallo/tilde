/* Copyright (C) 2012 G.P. Halkes
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 3, as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dialogs/attributesdialog.h"

#define START_WIDGET_GROUP { \
	widget_group_t *widget_group = new widget_group_t(); \
	int widget_count = 0;
#define END_WIDGET_GROUP(name, var) \
	widget_group->set_size(widget_count, width - 4); \
	var = new expander_t(name); \
	var->set_child(widget_group); \
	var->connect_move_focus_up(sigc::mem_fun(this, &attributes_dialog_t::focus_previous)); \
	var->connect_move_focus_down(sigc::mem_fun(this, &attributes_dialog_t::focus_next)); \
	expander_group->add_expander(var); \
}

#define ADD_ATTRIBUTE_ENTRY(name, sym, widget_name) do { \
	label = new smart_label_t(name); \
	label->set_position(widget_count, 0); \
	widget_group->add_child(label); \
	change_button = new button_t("Change"); \
	change_button->set_anchor(widget_group, T3_PARENT(T3_ANCHOR_TOPRIGHT) | T3_CHILD(T3_ANCHOR_TOPRIGHT)); \
	change_button->set_position(widget_count, 0); \
	change_button->connect_activate(sigc::bind(sigc::mem_fun(this, &attributes_dialog_t::change_button_activated), sym)); \
	change_button->connect_move_focus_up(sigc::mem_fun(widget_group, &widget_group_t::focus_previous)); \
	change_button->connect_move_focus_down(sigc::mem_fun(widget_group, &widget_group_t::focus_next)); \
	widget_group->add_child(change_button); \
	widget_name = new attribute_test_line_t(); \
	widget_name->set_anchor(change_button, T3_PARENT(T3_ANCHOR_TOPLEFT) | T3_CHILD(T3_ANCHOR_TOPRIGHT)); \
	widget_name->set_position(0, -2); \
	widget_group->add_child(widget_name); \
	widget_count++; \
} while (0)

//FIXME: we may be better of using a list_pane_t for the longer divisions
attributes_dialog_t::attributes_dialog_t(int width) : dialog_t(7, width, "Interface") {
	smart_label_t *label;
	button_t *change_button, *ok_button, *cancel_button;

	label = new smart_label_t(_("Color _mode"));
	label->set_position(1, 2);
	push_back(label);
	color_box = new checkbox_t();
	color_box->set_label(label);
	color_box->set_anchor(this, T3_PARENT(T3_ANCHOR_TOPRIGHT) | T3_CHILD(T3_ANCHOR_TOPRIGHT));
	color_box->set_position(1, -2);
	color_box->connect_move_focus_up(sigc::mem_fun(this, &attributes_dialog_t::focus_previous));
	color_box->connect_move_focus_down(sigc::mem_fun(this, &attributes_dialog_t::focus_next));
	color_box->connect_activate(sigc::mem_fun(this, &attributes_dialog_t::handle_activate));
	color_box->connect_toggled(sigc::mem_fun(this, &attributes_dialog_t::update_attribute_lines));


	expander_group = new expander_group_t();

	START_WIDGET_GROUP
	ADD_ATTRIBUTE_ENTRY("Dialog", DIALOG, dialog_line);
	ADD_ATTRIBUTE_ENTRY("Dialog selected", DIALOG_SELECTED, dialog_selected_line);
	ADD_ATTRIBUTE_ENTRY("Shadow", SHADOW, shadow_line);
	ADD_ATTRIBUTE_ENTRY("Background", BACKGROUND, background_line);
	ADD_ATTRIBUTE_ENTRY("Hotkey highlight", HOTKEY_HIGHLIGHT, hotkey_highlight_line);
	ADD_ATTRIBUTE_ENTRY("Badly drawn character", BAD_DRAW, bad_draw_line);
	ADD_ATTRIBUTE_ENTRY("Unprintable character", NON_PRINT, non_print_line);
	ADD_ATTRIBUTE_ENTRY("Button selected", BUTTON_SELECTED, button_selected_line);
	ADD_ATTRIBUTE_ENTRY("Scrollbar", SCROLLBAR, scrollbar_line);
	ADD_ATTRIBUTE_ENTRY("Menu bar", MENUBAR, menubar_line);
	ADD_ATTRIBUTE_ENTRY("Menu bar selected", MENUBAR_SELECTED, menubar_selected_line);
	END_WIDGET_GROUP("_Interface attributes", interface)
	interface->set_position(2, 2);

	START_WIDGET_GROUP
	ADD_ATTRIBUTE_ENTRY("Text", TEXT, text_line);
	ADD_ATTRIBUTE_ENTRY("Selected text", TEXT_SELECTED, text_selected_line);
	ADD_ATTRIBUTE_ENTRY("Cursor", TEXT_CURSOR, text_cursor_line);
	ADD_ATTRIBUTE_ENTRY("Selection cursor at end", TEXT_SELECTION_CURSOR, text_selection_cursor_line);
	ADD_ATTRIBUTE_ENTRY("Selection cursor at start", TEXT_SELECTION_CURSOR2, text_selection_cursor2_line);
	ADD_ATTRIBUTE_ENTRY("Meta text", META_TEXT, meta_text_line);
	ADD_ATTRIBUTE_ENTRY("Brace highlight", BRACE_HIGHLIGHT, brace_highlight_line);
	END_WIDGET_GROUP("_Text area attributes", text_area)
	text_area->set_anchor(interface, T3_PARENT(T3_ANCHOR_BOTTOMLEFT) | T3_CHILD(T3_ANCHOR_TOPLEFT));
	text_area->set_position(0, 0);

	START_WIDGET_GROUP
	ADD_ATTRIBUTE_ENTRY("Comment", COMMENT, comment_line);
	ADD_ATTRIBUTE_ENTRY("Comment keyword", COMMENT_KEYWORD, comment_keyword_line);
	ADD_ATTRIBUTE_ENTRY("Keyword", KEYWORD, keyword_line);
	ADD_ATTRIBUTE_ENTRY("Number", NUMBER, number_line);
	ADD_ATTRIBUTE_ENTRY("String", STRING, string_line);
	ADD_ATTRIBUTE_ENTRY("String escape", STRING_ESCAPE, string_escape_line);
	ADD_ATTRIBUTE_ENTRY("Miscelaneous", MISC, misc_line);
	ADD_ATTRIBUTE_ENTRY("Variable", VARIABLE, variable_line);
	ADD_ATTRIBUTE_ENTRY("Error", ERROR, error_line);
	ADD_ATTRIBUTE_ENTRY("Addition", ADDITION, addition_line);
	ADD_ATTRIBUTE_ENTRY("Deletion", DELETION, deletion_line);
	END_WIDGET_GROUP("_Syntax highlighting attributes", syntax_highlight)
	syntax_highlight->set_anchor(text_area, T3_PARENT(T3_ANCHOR_BOTTOMLEFT) | T3_CHILD(T3_ANCHOR_TOPLEFT));
	syntax_highlight->set_position(0, 0);

	expander_group->connect_expanded(sigc::mem_fun(this, &attributes_dialog_t::expander_size_change));


	cancel_button = new button_t("_Cancel");
	cancel_button->set_anchor(this, T3_PARENT(T3_ANCHOR_BOTTOMRIGHT) | T3_CHILD(T3_ANCHOR_BOTTOMRIGHT));
	cancel_button->set_position(-1, -2);
	cancel_button->connect_activate(sigc::mem_fun(this, &attributes_dialog_t::close));
	cancel_button->connect_move_focus_up(sigc::mem_fun(this, &attributes_dialog_t::focus_previous));
	cancel_button->connect_move_focus_up(sigc::mem_fun(this, &attributes_dialog_t::focus_previous));
	cancel_button->connect_move_focus_left(sigc::mem_fun(this, &attributes_dialog_t::focus_previous));

	ok_button = new button_t("_Ok", true);
	ok_button->set_anchor(cancel_button, T3_PARENT(T3_ANCHOR_TOPLEFT) | T3_CHILD(T3_ANCHOR_TOPRIGHT));
	ok_button->set_position(0, -2);
	ok_button->connect_move_focus_up(sigc::mem_fun(this, &attributes_dialog_t::focus_previous));
	ok_button->connect_move_focus_right(sigc::mem_fun(this, &attributes_dialog_t::focus_next));
	ok_button->connect_activate(sigc::mem_fun(this, &attributes_dialog_t::handle_activate));

	push_back(color_box);
	push_back(interface);
	push_back(text_area);
	push_back(syntax_highlight);
	push_back(ok_button);
	push_back(cancel_button);

	picker = new attribute_picker_dialog_t();
	picker->center_over(this);
	picker->connect_attribute_selected(sigc::mem_fun(this, &attributes_dialog_t::attribute_selected));
	picker->connect_default_selected(sigc::mem_fun(this, &attributes_dialog_t::default_attribute_selected));
}

bool attributes_dialog_t::set_size(optint height, optint width) {
	bool result = true;
	(void) height;
	if (!width.is_valid())
		return true;

	result &= interface->set_size(None, width);
	result &= text_area->set_size(None, width);
	result &= syntax_highlight->set_size(None, width);
	return result;
}

void attributes_dialog_t::show(void) {
	expander_group->collapse();
	dialog_t::show();
}

void attributes_dialog_t::change_button_activated(attribute_key_t attribute) {
	t3_attr_t text_attr;

	text_attr = text.is_valid() ? text() : get_default_attr(TEXT, color_box->get_state());
	change_attribute = attribute;

	switch (attribute) {
#define SET_WITH_DEFAULT(name, attr) case attr: \
	picker->set_base_attributes(0); \
	picker->set_attribute(name.is_valid() ? name() : get_default_attr(attr, color_box->get_state())); \
	break;
		SET_WITH_DEFAULT(dialog, DIALOG);
		SET_WITH_DEFAULT(dialog_selected, DIALOG_SELECTED);
		SET_WITH_DEFAULT(shadow, SHADOW);
		SET_WITH_DEFAULT(background, BACKGROUND);
		SET_WITH_DEFAULT(hotkey_highlight, HOTKEY_HIGHLIGHT);
		SET_WITH_DEFAULT(bad_draw, BAD_DRAW);
		SET_WITH_DEFAULT(non_print, NON_PRINT);
		SET_WITH_DEFAULT(button_selected, BUTTON_SELECTED);
		SET_WITH_DEFAULT(scrollbar, SCROLLBAR);
		SET_WITH_DEFAULT(menubar, MENUBAR);
		SET_WITH_DEFAULT(menubar_selected, MENUBAR_SELECTED);

		SET_WITH_DEFAULT(text, TEXT);
		SET_WITH_DEFAULT(text_selected, TEXT_SELECTED);
		SET_WITH_DEFAULT(text_cursor, TEXT_CURSOR);
		SET_WITH_DEFAULT(text_selection_cursor, TEXT_SELECTION_CURSOR);
		SET_WITH_DEFAULT(text_selection_cursor2, TEXT_SELECTION_CURSOR2);
#undef SET_WITH_DEFAULT


#define SET_WITH_DEFAULT(name, attr) case attr: \
	picker->set_base_attributes(text_attr); \
	picker->set_attribute(name.is_valid() ? name() : get_default_attr(attr, color_box->get_state())); \
	break;
		SET_WITH_DEFAULT(meta_text, META_TEXT);
		SET_WITH_DEFAULT(brace_highlight, BRACE_HIGHLIGHT);

		SET_WITH_DEFAULT(comment, COMMENT);
		SET_WITH_DEFAULT(comment_keyword, COMMENT_KEYWORD);
		SET_WITH_DEFAULT(keyword, KEYWORD);
		SET_WITH_DEFAULT(number, NUMBER);
		SET_WITH_DEFAULT(string, STRING);
		SET_WITH_DEFAULT(string_escape, STRING_ESCAPE);
		SET_WITH_DEFAULT(misc, MISC);
		SET_WITH_DEFAULT(variable, VARIABLE);
		SET_WITH_DEFAULT(error, ERROR);
		SET_WITH_DEFAULT(addition, ADDITION);
		SET_WITH_DEFAULT(deletion, DELETION);
#undef SET_WITH_DEFAULT
	}
	picker->show();
}

void attributes_dialog_t::expander_size_change(bool expanded) {
	(void) expanded;
	dialog_t::set_size(4 + expander_group->get_group_height(), None);
}

void attributes_dialog_t::set_values_from_options(void) {
	color_box->set_state(option.color);

	dialog = term_specific_option.dialog;
	dialog_selected = term_specific_option.dialog_selected;
	shadow = term_specific_option.shadow;
	background = term_specific_option.background;
	hotkey_highlight = term_specific_option.hotkey_highlight;
	bad_draw = term_specific_option.bad_draw;
	non_print = term_specific_option.non_print;
	button_selected = term_specific_option.button_selected;
	scrollbar = term_specific_option.scrollbar;
	menubar = term_specific_option.menubar;
	menubar_selected = term_specific_option.menubar_selected;

	text = term_specific_option.text;
	text_selected = term_specific_option.text_selected;
	text_cursor = term_specific_option.text_cursor;
	text_selection_cursor = term_specific_option.text_selection_cursor;
	text_selection_cursor2 = term_specific_option.text_selection_cursor2;
	meta_text = term_specific_option.meta_text;
	brace_highlight = term_specific_option.brace_highlight;

	comment = term_specific_option.highlights[map_highlight(NULL, "comment")];
	comment_keyword = term_specific_option.highlights[map_highlight(NULL, "comment_keyword")];
	keyword = term_specific_option.highlights[map_highlight(NULL, "keyword")];
	number = term_specific_option.highlights[map_highlight(NULL, "number")];
	string = term_specific_option.highlights[map_highlight(NULL, "string")];
	string_escape = term_specific_option.highlights[map_highlight(NULL, "string-escape")];
	misc = term_specific_option.highlights[map_highlight(NULL, "misc")];
	variable = term_specific_option.highlights[map_highlight(NULL, "variable")];
	error = term_specific_option.highlights[map_highlight(NULL, "error")];
	addition = term_specific_option.highlights[map_highlight(NULL, "addition")];
	deletion = term_specific_option.highlights[map_highlight(NULL, "deletion")];

	update_attribute_lines();
}

void attributes_dialog_t::set_options_from_values(void) {
	option.color = term_specific_option.color = color_box->get_state();

#define SET_WITH_DEFAULT(name, attr) do { \
	term_specific_option.name = name; \
	set_attribute(attribute_t::attr, name.is_valid() ? name() : get_default_attribute(attribute_t::attr, option.color)); \
} while (0)
	SET_WITH_DEFAULT(dialog, DIALOG);
	SET_WITH_DEFAULT(dialog_selected, DIALOG_SELECTED);
	SET_WITH_DEFAULT(shadow, SHADOW);
	SET_WITH_DEFAULT(background, BACKGROUND);
	SET_WITH_DEFAULT(hotkey_highlight, HOTKEY_HIGHLIGHT);
	SET_WITH_DEFAULT(bad_draw, BAD_DRAW);
	SET_WITH_DEFAULT(non_print, NON_PRINT);
	SET_WITH_DEFAULT(button_selected, BUTTON_SELECTED);
	SET_WITH_DEFAULT(scrollbar, SCROLLBAR);
	SET_WITH_DEFAULT(menubar, MENUBAR);
	SET_WITH_DEFAULT(menubar_selected, MENUBAR_SELECTED);
	SET_WITH_DEFAULT(text, TEXT);
	SET_WITH_DEFAULT(text_selected, TEXT_SELECTED);
	SET_WITH_DEFAULT(text_cursor, TEXT_CURSOR);
	SET_WITH_DEFAULT(text_selection_cursor, TEXT_SELECTION_CURSOR);
	SET_WITH_DEFAULT(text_selection_cursor2, TEXT_SELECTION_CURSOR2);
	SET_WITH_DEFAULT(meta_text, META_TEXT);
#undef SET_WITH_DEFAULT

	term_specific_option.brace_highlight = brace_highlight;
	option.brace_highlight = brace_highlight.is_valid() ? brace_highlight() : get_default_attr(BRACE_HIGHLIGHT);

#define SET_WITH_DEFAULT(name, attr) do { \
	int highlight_idx = map_highlight(NULL, #name); \
	term_specific_option.highlights[highlight_idx] = name; \
	option.highlights[highlight_idx] = name.is_valid() ? name() : get_default_attr(attr); \
} while (0)
	SET_WITH_DEFAULT(comment, COMMENT);
	SET_WITH_DEFAULT(comment_keyword, COMMENT_KEYWORD);
	SET_WITH_DEFAULT(keyword, KEYWORD);
	SET_WITH_DEFAULT(number, NUMBER);
	SET_WITH_DEFAULT(string, STRING);
	{
		int highlight_idx = map_highlight(NULL, "string-escape");
		term_specific_option.highlights[highlight_idx] = string_escape;
		option.highlights[highlight_idx] = string_escape.is_valid() ? string_escape() : get_default_attr(STRING_ESCAPE);
	}
	SET_WITH_DEFAULT(misc, MISC);
	SET_WITH_DEFAULT(variable, VARIABLE);
	SET_WITH_DEFAULT(error, ERROR);
	SET_WITH_DEFAULT(addition, ADDITION);
	SET_WITH_DEFAULT(deletion, DELETION);
#undef SET_WITH_DEFAULT
	option.highlights[0] = 0;
	force_redraw_all();
}

void attributes_dialog_t::update_attribute_lines(void) {
	t3_attr_t text_attr;
	bool color = color_box->get_state();

#define SET_WITH_DEFAULT(name, attr) name##_line->set_attribute(name.is_valid() ? name() : get_default_attr(attr, color))
	SET_WITH_DEFAULT(dialog, DIALOG);
	SET_WITH_DEFAULT(dialog_selected, DIALOG_SELECTED);
	SET_WITH_DEFAULT(shadow, SHADOW);
	SET_WITH_DEFAULT(background, BACKGROUND);
	SET_WITH_DEFAULT(hotkey_highlight, HOTKEY_HIGHLIGHT);
	SET_WITH_DEFAULT(bad_draw, BAD_DRAW);
	SET_WITH_DEFAULT(non_print, NON_PRINT);
	SET_WITH_DEFAULT(button_selected, BUTTON_SELECTED);
	SET_WITH_DEFAULT(scrollbar, SCROLLBAR);
	SET_WITH_DEFAULT(menubar, MENUBAR);
	SET_WITH_DEFAULT(menubar_selected, MENUBAR_SELECTED);

	SET_WITH_DEFAULT(text, TEXT);
	SET_WITH_DEFAULT(text_selected, TEXT_SELECTED);
	SET_WITH_DEFAULT(text_cursor, TEXT_CURSOR);
	SET_WITH_DEFAULT(text_selection_cursor, TEXT_SELECTION_CURSOR);
	SET_WITH_DEFAULT(text_selection_cursor2, TEXT_SELECTION_CURSOR2);
#undef SET_WITH_DEFAULT

	text_attr = text.is_valid() ? text() : get_default_attr(TEXT, color);

#define SET_WITH_DEFAULT(name, attr) name##_line->set_attribute(t3_term_combine_attrs(name.is_valid() ? name() : get_default_attr(attr, color), text_attr))
	SET_WITH_DEFAULT(meta_text, META_TEXT);
	SET_WITH_DEFAULT(brace_highlight, BRACE_HIGHLIGHT);

	SET_WITH_DEFAULT(comment, COMMENT);
	SET_WITH_DEFAULT(comment_keyword, COMMENT_KEYWORD);
	SET_WITH_DEFAULT(keyword, KEYWORD);
	SET_WITH_DEFAULT(number, NUMBER);
	SET_WITH_DEFAULT(string, STRING);
	SET_WITH_DEFAULT(string_escape, STRING_ESCAPE);
	SET_WITH_DEFAULT(misc, MISC);
	SET_WITH_DEFAULT(variable, VARIABLE);
	SET_WITH_DEFAULT(error, ERROR);
	SET_WITH_DEFAULT(addition, ADDITION);
	SET_WITH_DEFAULT(deletion, DELETION);
#undef SET_WITH_DEFAULT
}

void attributes_dialog_t::attribute_selected(t3_attr_t attribute) {
	t3_attr_t text_attr;
	text_attr = text.is_valid() ? text() : get_default_attr(TEXT, color_box->get_state());

	switch (change_attribute) {
#define SET_WITH_DEFAULT(name, attr) case attr: \
	name = attribute; \
	name##_line->set_attribute(attribute); \
	break;
		SET_WITH_DEFAULT(dialog, DIALOG);
		SET_WITH_DEFAULT(dialog_selected, DIALOG_SELECTED);
		SET_WITH_DEFAULT(shadow, SHADOW);
		SET_WITH_DEFAULT(background, BACKGROUND);
		SET_WITH_DEFAULT(hotkey_highlight, HOTKEY_HIGHLIGHT);
		SET_WITH_DEFAULT(bad_draw, BAD_DRAW);
		SET_WITH_DEFAULT(non_print, NON_PRINT);
		SET_WITH_DEFAULT(button_selected, BUTTON_SELECTED);
		SET_WITH_DEFAULT(scrollbar, SCROLLBAR);
		SET_WITH_DEFAULT(menubar, MENUBAR);
		SET_WITH_DEFAULT(menubar_selected, MENUBAR_SELECTED);

		SET_WITH_DEFAULT(text, TEXT);
		SET_WITH_DEFAULT(text_selected, TEXT_SELECTED);
		SET_WITH_DEFAULT(text_cursor, TEXT_CURSOR);
		SET_WITH_DEFAULT(text_selection_cursor, TEXT_SELECTION_CURSOR);
		SET_WITH_DEFAULT(text_selection_cursor2, TEXT_SELECTION_CURSOR2);

#undef SET_WITH_DEFAULT

#define SET_WITH_DEFAULT(name, attr) case attr: \
	name = attribute; \
	name##_line->set_attribute(t3_term_combine_attrs(attribute, text_attr)); \
	break;
		SET_WITH_DEFAULT(meta_text, META_TEXT);
		SET_WITH_DEFAULT(brace_highlight, BRACE_HIGHLIGHT);

		SET_WITH_DEFAULT(comment, COMMENT);
		SET_WITH_DEFAULT(comment_keyword, COMMENT_KEYWORD);
		SET_WITH_DEFAULT(keyword, KEYWORD);
		SET_WITH_DEFAULT(number, NUMBER);
		SET_WITH_DEFAULT(string, STRING);
		SET_WITH_DEFAULT(string_escape, STRING_ESCAPE);
		SET_WITH_DEFAULT(misc, MISC);
		SET_WITH_DEFAULT(variable, VARIABLE);
		SET_WITH_DEFAULT(error, ERROR);
		SET_WITH_DEFAULT(addition, ADDITION);
		SET_WITH_DEFAULT(deletion, DELETION);
#undef SET_WITH_DEFAULT
	}
	picker->hide();
}

void attributes_dialog_t::default_attribute_selected(void) {
	t3_attr_t text_attr;
	text_attr = text.is_valid() ? text() : get_default_attr(TEXT, color_box->get_state());

	switch (change_attribute) {
#define SET_DEFAULT(name, attr) case attr: \
	name.unset(); \
	name##_line->set_attribute(get_default_attr(attr, color_box->get_state())); \
	break;
		SET_DEFAULT(dialog, DIALOG);
		SET_DEFAULT(dialog_selected, DIALOG_SELECTED);
		SET_DEFAULT(shadow, SHADOW);
		SET_DEFAULT(background, BACKGROUND);
		SET_DEFAULT(hotkey_highlight, HOTKEY_HIGHLIGHT);
		SET_DEFAULT(bad_draw, BAD_DRAW);
		SET_DEFAULT(non_print, NON_PRINT);
		SET_DEFAULT(button_selected, BUTTON_SELECTED);
		SET_DEFAULT(scrollbar, SCROLLBAR);
		SET_DEFAULT(menubar, MENUBAR);
		SET_DEFAULT(menubar_selected, MENUBAR_SELECTED);

		SET_DEFAULT(text, TEXT);
		SET_DEFAULT(text_selected, TEXT_SELECTED);
		SET_DEFAULT(text_cursor, TEXT_CURSOR);
		SET_DEFAULT(text_selection_cursor, TEXT_SELECTION_CURSOR);
		SET_DEFAULT(text_selection_cursor2, TEXT_SELECTION_CURSOR2);

#undef SET_DEFAULT

#define SET_DEFAULT(name, attr) case attr: \
	name.unset(); \
	name##_line->set_attribute(t3_term_combine_attrs(get_default_attr(attr, color_box->get_state()), text_attr)); \
	break;
		SET_DEFAULT(meta_text, META_TEXT);
		SET_DEFAULT(brace_highlight, BRACE_HIGHLIGHT);

		SET_DEFAULT(comment, COMMENT);
		SET_DEFAULT(comment_keyword, COMMENT_KEYWORD);
		SET_DEFAULT(keyword, KEYWORD);
		SET_DEFAULT(number, NUMBER);
		SET_DEFAULT(string, STRING);
		SET_DEFAULT(string_escape, STRING_ESCAPE);
		SET_DEFAULT(misc, MISC);
		SET_DEFAULT(variable, VARIABLE);
		SET_DEFAULT(error, ERROR);
		SET_DEFAULT(addition, ADDITION);
		SET_DEFAULT(deletion, DELETION);
#undef SET_DEFAULT
	}
	picker->hide();
}

void attributes_dialog_t::handle_activate(void) {
	/* Do required validation here. */
	hide();
	activate();
}