#ifndef REPORT_H__
#define REPORT_H__
#pragma once

void Reprint(struct lemon *);
void ReportOutput(struct lemon *);
void ReportTable(struct lemon *, int);
void ReportHeader(struct lemon *);
void CompressTables(struct lemon *);
void ResortStates(struct lemon *);

#endif	//REPORT_H__
