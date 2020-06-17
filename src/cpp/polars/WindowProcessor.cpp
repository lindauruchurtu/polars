//
// Created by Calvin Giles on 2018-10-02.
//

#include "WindowProcessor.h"

#include "Series.h"
#include "numc.h"


namespace polars {


    // check if a double is an integer value, e.g. 2.0 vs 2.1
    bool double_is_int(double v) {
        double intpart;
        return modf(v, &intpart) == 0;
    }


    Quantile::Quantile(double quantile) : quantile(quantile) {
        //assert(quantile >= 0);
        //assert(quantile <= 1);
    }


    double Quantile::processWindow(const Series &window, const arma::vec& weights) const {

        arma::vec v;
        v = weights % window.values();
        v = sort(v.elem(arma::find_finite(v)));

        // note, this is based on how q works in python numpy percentile rather than the more usual quantile defn.
        double quantilePosition = quantile * ((double) v.size() - 1);

        if (double_is_int(quantilePosition)) {
            return v(quantilePosition);
        } else {
            // interpolate estimate
            arma::uword quantileIdx = floor(quantilePosition);
            double fraction = quantilePosition - quantileIdx;
            return v(quantileIdx) + (v(quantileIdx + 1) - v(quantileIdx)) * fraction;
        }
    }


    double polars::Sum::processWindow(const Series &window, const arma::vec& weights) const {
        return polars::numc::sum_finite((weights % window.values()));
    }


    polars::Count::Count(double default_value) : default_value(default_value) {}


    double polars::Count::processWindow(const Series &window, const arma::vec& weights) const {
        return window.finiteSize();
    }


    polars::Mean::Mean(double default_value) : default_value(default_value) {}


    double polars::Mean::processWindow(const Series &window, const arma::vec& weights) const {
        // This ensures deals with NAs like pandas for the case ignore_na = False which is the default setting.
        arma::vec weights_for_sum = weights.elem(arma::find_finite(window.values()));
        arma::vec weighted_values = window.values() % weights;
        return polars::numc::sum_finite(weighted_values) / arma::sum(weights_for_sum);
    }


    polars::Std::Std(double default_value) : default_value(default_value) {}


    double polars::Std::processWindow(const Series &window, const arma::vec& weights) const {
        auto weighted_series = Series(window.values() % weights, window.index());
        return weighted_series.std();
    }


    double polars::ExpMean::processWindow(const Series &window, const arma::vec& weights) const {
        // This ensures deals with NAs like pandas for the case ignore_na = False which is the default setting.
        arma::vec weights_for_sum = weights.elem(arma::find_finite(window.values()));
        arma::vec weighted_values = window.values() % weights;
        return polars::numc::sum_finite(weighted_values) / arma::sum(weights_for_sum);
    }

    Series Rolling::count() {
        return ts_.rolling(windowSize_, Count(), minPeriods_, center_, symmetric_);
    }

    Series Rolling::sum() {
        return ts_.rolling(windowSize_, Sum(), minPeriods_, center_, symmetric_);
    }

    Series Rolling::mean() {
        return ts_.rolling(windowSize_, Mean(), minPeriods_, center_, symmetric_);
    }

    Series Rolling::std() {
        return ts_.rolling(windowSize_, Std(), minPeriods_, center_, symmetric_);
    }

    Series Rolling::quantile(double q) {
        return ts_.rolling(windowSize_, Quantile(q), minPeriods_, center_, symmetric_);
    }

    Series Rolling::min() {
        return ts_.rolling(windowSize_, Quantile(0.), minPeriods_, center_, symmetric_);
    }

    Series Rolling::max() {
        return ts_.rolling(windowSize_, Quantile(1.), minPeriods_, center_, symmetric_);
    }

    Series Rolling::median() {
        return ts_.rolling(windowSize_, Quantile(0.5), minPeriods_, center_, symmetric_);
    }

    Series Window::mean() {
        return ts_.rolling(windowSize_, Mean(), minPeriods_, center_, symmetric_, win_type_);
    }

    Series Window::sum() {
        return ts_.rolling(windowSize_, Sum(), minPeriods_, center_, symmetric_, win_type_);
    }

} // polars